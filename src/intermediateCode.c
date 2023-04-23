#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "intermediateCode.h"
#include "parser.tab.h"

extern void yyerror();
extern void icerror();

int tempCount = 0;
int labelCount = 0;
int newTemp(){ return tempCount++; }
int newLabel(){ return labelCount++; }
void popTemp(int n){ tempCount -= n; }

InstrList list;
DataList dataList;
tmpTable ttable;

int funTmp;
int localTmp;
int isFun = 0;

void initTmpTable(){
	int t = newTemp();
	ttable = tmpPush(ttable, "readint",t);
	t = newTemp();
	ttable = tmpPush(ttable, "writeint",t);
	t = newTemp();
	ttable = tmpPush(ttable, "writestring",t);
}

int inLoop = -1;

InstrList addInstr(InstrList l, Instr i){
	InstrList cur = (InstrList)malloc(sizeof(struct _instrlist));
	InstrList add = (InstrList)malloc(sizeof(struct _instrlist));
	cur = l;
	if(NULL == cur){ add->in = i; return add;  }
	while(NULL != cur->next){
		cur = cur->next;
	}
	add->in = i;
	cur->next = add;
	return l;
}

ParamList addParam(ParamList l, int i){
	ParamList cur = (ParamList)malloc(sizeof(struct _paramlist));
	ParamList add = (ParamList)malloc(sizeof(struct _paramlist));
	cur = l;
	if(NULL == cur){ add->value = i; return add;  }
	while(NULL != cur->next){
		cur = cur->next;
	}
	add->value = i;
	cur->next = add;
	return l;
}

DataList addStr(DataList l, char* var, char* s){
	DataList cur = (DataList)malloc(sizeof(struct _datalist));
	DataList add = (DataList)malloc(sizeof(struct _datalist));
	cur = l;
	if(NULL == cur){ add->id = var; add->string = s; return add; }
	while(NULL != cur->next){
		cur = cur->next;
	}
	add->id = var; add->string = s;
	cur->next = add;
	return l;
}

void emit1(int op, int l){
	Instr in = (Instr)malloc(sizeof(Instr));
	in->opcode = op;
	in->arg1 = l;
	list = addInstr(list,in);
}

void emit2(int op, int d, int t1, char* str){
	Instr in = (Instr)malloc(sizeof(Instr));
	in->opcode = op;
	in->arg1 = d;
	in->arg2 = t1;
	in->strLiteral = str;
	list = addInstr(list,in);
}

void emit3(int op, int d, int t1, int t2){
	Instr in = (Instr)malloc(sizeof(struct _instr));
	in->opcode = op;
	in->arg1 = d;
	in->arg2 = t1;
	in->arg3 = t2;
	list = addInstr(list,in);
}

void emit4(int op, int t1, int t2, int l1, int l2){
	Instr in = (Instr)malloc(sizeof(struct _instr));
	in->opcode = op;
	in->arg1 = t1;
	in->arg2 = t2;
	in->arg3 = l1;
	in->arg4 = l2;
	list = addInstr(list,in);
}

void emitFun(int op, int dest, char* id, ParamList params){
	Instr in = (Instr)malloc(sizeof(struct _instr));
	in->opcode = op;
	in->arg1 = dest;
	in->ident = id;
	in->list = params;
	list = addInstr(list,in);
}

void transProgram(Program prog){
	if(!prog){ return; }
	switch(prog->tag){
		case PROGRAM:
			transProgram(prog->program.body);
			break;
		case PROGRAM_BODY:
			transDecl(prog->programBody.constDecl);
			ProcList cur = (ProcList)malloc(sizeof(struct _procedure_list));
			cur = prog->programBody.list;
			while(NULL != cur){
				transProc(cur->proc);
				cur = cur->next;
			}
			transDecl(prog->programBody.varDecl);
			transStm(prog->programBody.stm);
			break;
		default:
			icerror("unable to generate code for program");
	}
}

void transProc(Proc proc){
	if(!proc){ return; }
	switch(proc->tag){
		case PROC:
			transProc(proc->proc.header);
			transProc(proc->proc.body);
			break;
		case PROC_HEADER:{
			funTmp = newTemp(); int t;
			localTmp = tempCount;
			isFun = 0;
			ttable = tmpPush(ttable, proc->procHeader.ident, funTmp);
			ProcList cur = (ProcList)malloc(sizeof(struct _procedure_list));
			cur = proc->procHeader.list;
			ParamList list = (ParamList)malloc(sizeof(struct _paramlist));
			while(NULL != cur){
				t = newTemp();
				ttable = tmpPush(ttable, cur->proc->param.ident, t);
				list = addParam(list, t);
				cur = cur->next;
			}
			emitFun(OPCODE_DEF, funTmp, proc->procHeader.ident, list);
			break;
		}
		case FUN_HEADER:{
			funTmp = newTemp(); int t;
			localTmp = tempCount;
			isFun = 1;
			ttable = tmpPush(ttable, proc->funHeader.ident, funTmp);
			ProcList cur = (ProcList)malloc(sizeof(struct _procedure_list));
			cur = proc->funHeader.list;
			ParamList list = (ParamList)malloc(sizeof(struct _paramlist));
			while(NULL != cur){
				t = newTemp();
				ttable = tmpPush(ttable, cur->proc->param.ident, t);
				list = addParam(list, t);
				cur = cur->next;
			}
			emitFun(OPCODE_DEF, funTmp, proc->funHeader.ident, list);
			break;
		}
		case PROC_BODY:
			transDecl(proc->procBody.decl);
			transStm(proc->procBody.stm);
			if(isFun){emitFun(OPCODE_RETURN, funTmp*isFun, NULL, NULL);}
			popTemp(tempCount-localTmp);
			break;
		default:
			icerror("unable to generate code for procedure or function");
	}
}

void transDecl(Decl decl){
	if(!decl){ return; }
	switch(decl->tag){
		case CONST_DECL:{
			DeclList cur = (DeclList)malloc(sizeof(struct _declaration_list));
			cur = decl->constDecl.list;
			while(NULL != cur){
				transDecl(cur->decl);
				cur = cur->next;
			}
			break;
		}
		case VAR_DECL:{
			DeclList cur = (DeclList)malloc(sizeof(struct _declaration_list));
			cur = decl->varDecl.list;
			while(NULL != cur){
				transDecl(cur->decl);
				cur = cur->next;
			}
			break;
		}
		case CONST_DEF:{
			int tConst = newTemp();
			ttable = tmpPush(ttable, decl->constDef.ident, tConst);
			Exp assignVar = (Exp)malloc(sizeof(struct _expression));
			assignVar->tag = VAR_ACCESS;
			assignVar->varAccess.ident = decl->constDef.ident;
			Exp assignValue = (Exp)malloc(sizeof(struct _expression));
			assignValue->tag = NUM;
			assignValue->num = decl->constDef.num;
			Stm assignToConst = (Stm)malloc(sizeof(struct _statement));
			assignToConst->tag = ASSIGN;
			assignToConst->assignStm.varA = assignVar;
			assignToConst->assignStm.exp = assignValue;
			transStm(assignToConst);
			break;
		}
		case VAR_DEF:{
			int tVar = newTemp();
			ttable = tmpPush(ttable, decl->varDef.ident, tVar);
			break;
		}
		default:
			icerror("unable to generate code for declaration");
	}
}

void transStm(Stm s){
	if(!s){ return; }
	switch(s->tag){
		case ASSIGN:{
			tmpTable tmp  = tmpLookup(ttable, s->assignStm.varA->varAccess.ident);
			if(NULL == tmp){ icerror("trying to access undeclared variable"); }
			int dest = tmp->tmpValue;
			transExp(s->assignStm.exp, dest);
			if(s->assignStm.exp->tag == STR){
				char* str = malloc(strlen( s->assignStm.exp->str)+1); strcpy(str, s->assignStm.exp->str);
				str[0] = '"';
				str[strlen(str)-1] = '"';
				dataList = addStr(dataList, s->assignStm.varA->varAccess.ident, str);
			}
			break;
		}
		case IF:{
			if(NULL == s->ifStm.elseStm){ // case if a then b
				int l1 = newLabel();
				int l2 = newLabel();
				transCond(s->ifStm.exp, l1, l2);
				emit1(OPCODE_LABEL, l1);
				transStm(s->ifStm.thenStm);
				emit1(OPCODE_LABEL, l2);
			}
			else{ // case if a then b else c
				int l1 = newLabel();
				int l2 = newLabel();
				int l3 = newLabel();
				transCond(s->ifStm.exp, l1, l2);
				emit1(OPCODE_LABEL, l1);
				transStm(s->ifStm.thenStm);
				emit1(OPCODE_JUMP, l3);
				emit1(OPCODE_LABEL, l2);
				transStm(s->ifStm.elseStm);
				emit1(OPCODE_LABEL, l3);
			}
			break;
		}
		case WHILE:{
			int l1 = newLabel();
			int l2 = newLabel();
			int l3 = newLabel();
			inLoop = l3;
			emit1(OPCODE_LABEL, l1);
			transCond(s->whileStm.exp, l2, l3);
			emit1(OPCODE_LABEL, l2);
			transStm(s->whileStm.doStm);
			emit1(OPCODE_JUMP, l1);
			emit1(OPCODE_LABEL, l3);
			inLoop = -1;
			break;
		}
		case FOR:{

			// Translate for into while
			tmpTable tmp = tmpLookup(ttable, s->forStm.ident);
			if(NULL == tmp){ icerror("trying to access undeclared variable"); }
			int dest = tmp->tmpValue;
			transExp(s->forStm.expFrom, dest);
			Exp iter = (Exp)malloc(sizeof(struct _expression));
			iter->tag = VAR_ACCESS;
			iter->varAccess.ident = s->forStm.ident;
			Exp cond = (Exp)malloc(sizeof(struct _expression));
			cond->tag = BINOP_COMP;
			cond->binopComp.op = BINOP_LT;
			cond->binopComp.left = iter;
			cond->binopComp.right = s->forStm.expTo;
			Exp incrExp = (Exp)malloc(sizeof(struct _expression));
			incrExp->tag = BINOP_INT;
			incrExp->binopInt.op = BINOP_PLUS;
			incrExp->binopInt.left = iter;
			Exp addOne = (Exp)malloc(sizeof(struct _expression));
			addOne->tag = NUM;
			addOne->num = 1;
			incrExp->binopInt.right = addOne;
			Stm incrStm = (Stm)malloc(sizeof(struct _statement));
			incrStm->tag = ASSIGN;
			incrStm->assignStm.varA = iter;
			incrStm->assignStm.exp = incrExp;

			// Evaluate as While
			int l1 = newLabel();
			int l2 = newLabel();
			int l3 = newLabel();
			inLoop = l3;
			emit1(OPCODE_LABEL, l1);
			transCond(cond, l2, l3);
			emit1(OPCODE_LABEL, l2);
			transStm(s->forStm.doStm);
			transStm(incrStm);
			emit1(OPCODE_JUMP, l1);
			emit1(OPCODE_LABEL, l3);
			inLoop = -1;
			break;
		}
		case BREAK:
			if(inLoop == -1){ yyerror("break statement outside loop"); }
			emit1(OPCODE_JUMP, inLoop);
			break;
		case PROC_STM:{
			int t = newTemp();
			transFunCall(s->procStm.ident, s->procStm.list, t);
			break;
		}
		case COMPOUND:{
			StmList cur = s->compoundStm.list;
			while(NULL != cur){ transStm(cur->stm); cur = cur->next; }
			break;
		}
		default:
			icerror("unable to generate code for statement");
	}
}

void transCond(Exp e, int l1, int l2){
	if(!e){ return; }
	switch(e->tag){
		case BOOL:
			if (strcmp(e->boolVal,"true") == 0){ emit1(OPCODE_JUMP, l1); }
			else if (strcmp(e->boolVal,"false") == 0){ emit1(OPCODE_JUMP, l2); }
			else{ icerror("invalid boolean value"); }
			break;
		case BINOP_COMP:{
			int t1 = newTemp();
			int t2 = newTemp();
			transExp(e->binopComp.left, t1);
			transExp(e->binopComp.right, t2);
			emit4(getCompOpcode(e->binopComp.op), t1, t2, l1, l2);
			break;
		}
		case BINOP_BOOL:
			switch(e->binopBool.op){
				case UNOP_NOT:
					transCond(e->binopBool.right, l2, l1);
					break;
				case BINOP_AND:{
					int l = newLabel();
					transCond(e->binopBool.left, l, l2);
					emit1(OPCODE_LABEL, l);
					transCond(e->binopBool.right, l1, l2);
					break;
				}
				case BINOP_OR:{
					int l = newLabel();
					transCond(e->binopBool.left, l1, l);
					emit1(OPCODE_LABEL, l);
					transCond(e->binopBool.right, l1, l2);
					break;
				}
				default:
					icerror("unrecognized boolean operator");
			}
			break;
		case VAR_ACCESS:{
			tmpTable tmp = tmpLookup(ttable,e->varAccess.ident);
			if(NULL == tmp){ icerror("variable was not assigned a temporary value"); }
			int t = newTemp();
			transExp(e, t);
			emit4(OPCODE_NOTEQ, t, -1, l1, l2);
			break;
		}
		case FUN_ACCESS:{
			tmpTable tmp = tmpLookup(ttable,e->expList.ident);
			if(NULL == tmp){ icerror("variable was not assigned a temporary value"); }
			int t = newTemp();
			transExp(e, t);
			emit4(OPCODE_NOTEQ, t, -1, l1, l2);
			break;
		}
		default:
			icerror("unable to generate code for condition");
	}
}

void transExp(Exp e, int dest){
	if(!e){ return; }
	switch(e->tag){
		case NUM:
			emit2(OPCODE_MOVEI, dest, e->num, NULL);
			break;
		case STR:{
			emit2(OPCODE_MOVES, dest, -1, e->str);
			break;
		}
		case BOOL:{
			int flag = -1;
			if (strcmp(e->boolVal,"true") == 0){ flag = 1; }
			else if (strcmp(e->boolVal,"false") == 0){ flag = 0; }
			else{ icerror("invalid boolean value"); }
			emit2(OPCODE_MOVEB, dest, flag, NULL);
			break;
		}
		case VAR_ACCESS:{
			tmpTable t = tmpLookup(ttable,e->varAccess.ident);
			if(NULL == t){ icerror("variable was not assigned a temporary value"); }
			emit2(OPCODE_MOVE, dest, t->tmpValue, NULL);
			break;
		}
		case FUN_ACCESS:{
			transFunCall(e->expList.ident, e->expList.list, dest);
			break;
		}
		case BINOP_INT:{
			int t1 = newTemp();
			int t2 = newTemp();
			transExp(e->binopInt.left, t1);
			transExp(e->binopInt.right, t2);
			popTemp(2);
			emit3(getIntOpcode(e->binopInt.op),dest,t1,t2); // dest := t1 op t2
			break;
		}
		case BINOP_COMP:{
			int l1 = newLabel();
			int l2 = newLabel();
			int l3 = newLabel();
			transCond(e, l1, l2);
			emit1(OPCODE_LABEL, l1);
			emit2(OPCODE_MOVEI, dest, 1, NULL);
			emit1(OPCODE_JUMP, l3);
			emit1(OPCODE_LABEL, l2);
			emit2(OPCODE_MOVEI, dest, 0, NULL);
			emit1(OPCODE_LABEL, l3);
			break;
		}
		case BINOP_BOOL:{
			int l1 = newLabel();
			int l2 = newLabel();
			int l3 = newLabel();
			transCond(e, l1, l2);
			emit1(OPCODE_LABEL, l1);
			emit2(OPCODE_MOVEI, dest, 1, NULL);
			emit1(OPCODE_JUMP, l3);
			emit1(OPCODE_LABEL, l2);
			emit2(OPCODE_MOVEI, dest, 0, NULL);
			emit1(OPCODE_LABEL, l3);
			break;
		}
		default:
			icerror("unable to generate code for expression");
	}
}

void transFunCall(char* id, ExpList list, int dest){
	ExpList cur = (ExpList)malloc(sizeof(struct _expression_list));
	cur = list;
	ParamList tmpList = (ParamList)malloc(sizeof(struct _paramlist));
	if(NULL == cur){ emitFun(OPCODE_CALL, dest, id, tmpList); }
	else{
		while(NULL != cur){
			int t = newTemp();
			tmpList = addParam(tmpList, t);
			transExp(cur->exp,t);
			cur = cur->next;
		}
		emitFun(OPCODE_CALL, dest, id, tmpList);
	}
}

int getIntOpcode(int op){
	switch(op){
		case BINOP_PLUS:
			return OPCODE_ADD;
		case BINOP_MULT:
			return OPCODE_MULT;
		case BINOP_MINUS:
			return OPCODE_SUB;
		case BINOP_DIV:
			return OPCODE_DIV;
		case BINOP_MOD:
			return OPCODE_MOD;
		default:
			icerror("invalid opcode");
	}
}

int getCompOpcode(int op){
	switch(op){
		case BINOP_EQ:
			return OPCODE_EQ;
		case BINOP_NOTEQ:
			return OPCODE_NOTEQ;
		case BINOP_LT:
			return OPCODE_LT;
		case BINOP_LEQT:
			return OPCODE_LEQT;
		case BINOP_GT:
			return OPCODE_GT;
		case BINOP_GEQT:
			return OPCODE_GEQT;
		default:
			icerror("invalid opcode");
	}
}

InstrList getInstrList(){ return list; }
DataList getDataList(){ return dataList; }
tmpTable getTmpTable(){ return ttable; }

void printInstructionList(InstrList list){
	InstrList cur = (InstrList)malloc(sizeof(struct _instrlist));
	cur = list;
	while(NULL != cur){
		printInstr(cur->in);
		cur = cur->next;
	}
}

void printInstr(Instr in){
	if(!in){ icerror("cannot print empty instruction"); }
	switch(in->opcode){
		case OPCODE_DEF:
			printf("%s(", in->ident); printParams(in->list); printf(") [\n");
			break;
		case OPCODE_CALL:
			printTmp(in->arg1); printf(" := CALL %s(", in->ident); printParams(in->list); printf(")\n");
			break;
		case OPCODE_RETURN:
			if(in->arg1){
				printf("RETURN "); printTmp(in->arg1); printf("\n]\n");
			}
			else{ printf("]\n"); }
			break;
		case OPCODE_LABEL:
			printf("LABEL "); printLabel(in->arg1); printf("\n");
			break;
		case OPCODE_JUMP:
			printf("JUMP "); printLabel(in->arg1); printf("\n");
			break;
		case OPCODE_MOVE:
			printTmp(in->arg1); printf(" := "); printTmp(in->arg2); printf("\n");
			break;
		case OPCODE_MOVEI:
			printTmp(in->arg1); printf(" := "); printf("%d\n", in->arg2);
			break;
		case OPCODE_MOVES:
			printTmp(in->arg1); printf(" := "); printf("%s\n", in->strLiteral);
			break;
		case OPCODE_MOVEB:
			printTmp(in->arg1); printf(" := "); printf("%d\n", in->arg2);
			break;
		case OPCODE_ADD:
			printTmp(in->arg1); printf(" := "); printTmp(in->arg2); printf(" + "); printTmp(in->arg3); printf("\n");
			break;
		case OPCODE_SUB:
			printTmp(in->arg1); printf(" := "); printTmp(in->arg2); printf(" - "); printTmp(in->arg3); printf("\n");
			break;
		case OPCODE_MULT:
			printTmp(in->arg1); printf(" := "); printTmp(in->arg2); printf(" * "); printTmp(in->arg3); printf("\n");
			break;
		case OPCODE_DIV:
			printTmp(in->arg1); printf(" := "); printTmp(in->arg2); printf(" / "); printTmp(in->arg3); printf("\n");
			break;
		case OPCODE_MOD:
			printTmp(in->arg1); printf(" := "); printTmp(in->arg2); printf(" % "); printTmp(in->arg3); printf("\n");
			break;
		case OPCODE_EQ:
			printf("COND "); printTmp(in->arg1); printf(" = "); printTmp(in->arg2); printf(" "); printLabel(in->arg3); printf(" "); printLabel(in->arg4); printf("\n");
			break;
		case OPCODE_NOTEQ:
			printf("COND "); printTmp(in->arg1); printf(" != "); printTmp(in->arg2); printf(" "); printLabel(in->arg3); printf(" "); printLabel(in->arg4); printf("\n");
			break;
		case OPCODE_LT:
			printf("COND "); printTmp(in->arg1); printf(" < "); printTmp(in->arg2); printf(" "); printLabel(in->arg3); printf(" "); printLabel(in->arg4); printf("\n");
			break;
		case OPCODE_LEQT:
			printf("COND "); printTmp(in->arg1); printf(" <= "); printTmp(in->arg2); printf(" "); printLabel(in->arg3); printf(" "); printLabel(in->arg4); printf("\n");
			break;
		case OPCODE_GT:
			printf("COND "); printTmp(in->arg1); printf(" > "); printTmp(in->arg2); printf(" "); printLabel(in->arg3); printf(" "); printLabel(in->arg4); printf("\n");
			break;
		case OPCODE_GEQT:
			printf("COND "); printTmp(in->arg1); printf(" >= "); printTmp(in->arg2); printf(" "); printLabel(in->arg3); printf(" "); printLabel(in->arg4); printf("\n");
			break;
		default:
			icerror("cannot print operation with invalid opcode");
	}
}

void printTmp(int t){
	if(t == -1){ printf("0"); }
	else{ printf("t%d",t); }
}

void printLabel(int l){
	printf("label%d",l);
}

void printParams(ParamList l){
	ParamList cur = (ParamList)malloc(sizeof(struct _paramlist));
	cur = l->next;
	if(NULL == cur){ return; }
	if(NULL == cur->next){ printf("t%d", cur->value); return; }
	printf("t%d,", cur->value);
	cur = cur->next;
	while(NULL != cur->next){
		printf("t%d,", cur->value);
		cur = cur->next;
	}
	printf("t%d", cur->value);
}
