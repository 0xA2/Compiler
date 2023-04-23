#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"
#include "symTable.h"

extern void yyerror();

// -- Node construction -- //

// Program

Program astProgramMakeProgram(Program h, Program b){
	Program ret = (Program)malloc(sizeof(struct _program));
	ret->tag = PROGRAM;
	ret->program.header = h;
	ret->program.body = b;
	return ret;
}

Program astProgramMakeHeader(char* id){
	Program ret = (Program)malloc(sizeof(struct _program));
	ret->tag = PROGRAM_HEADER;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->programHeader.ident = s;
	return ret;
}

Program astProgramMakeBody(Decl cD, ProcList pl, Decl vD, Stm st){
	Program ret = (Program)malloc(sizeof(struct _program));
	ret->tag = PROGRAM_BODY;
	ret->programBody.constDecl = cD;
	ret->programBody.list = pl;
	ret->programBody.varDecl = vD;
	ret->programBody.stm = st;
	return ret;
}

// Procedures and Functions

Proc astProcMakeProc(Proc h, Proc b){
	Proc ret = (Proc)malloc(sizeof(struct _procedure));
	ret->tag = PROC;
	ret->proc.header = h;
	ret->proc.body = b;
	return ret;
}

Proc astProcMakeProcHeader(char* id, ProcList pl){
	Proc ret = (Proc)malloc(sizeof(struct _procedure));
	ret->tag = PROC_HEADER;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->procHeader.ident = s;
	ret->procHeader.list = pl;
	return ret;
}

Proc astProcMakeFunHeader(char* id, ProcList pl, Decl t){
	Proc ret = (Proc)malloc(sizeof(struct _procedure));
	ret->tag = FUN_HEADER;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->funHeader.ident = s;
	ret->funHeader.list = pl;
	ret->funHeader.type = t;
	return ret;
}

Proc astProcMakeProcBody(Decl d, Stm s){
	Proc ret = (Proc)malloc(sizeof(struct _procedure));
	ret->tag = PROC_BODY;
	ret->procBody.decl = d;
	ret->procBody.stm = s;
	return ret;
}

Proc astProcMakeParam(char* id, Decl t){
	Proc ret = (Proc)malloc(sizeof(struct _procedure));
	ret->tag = PARAM;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->param.ident = s;
	ret->param.type = t;
	return ret;
}

ProcList astProcList(Proc p, ProcList pl){
	ProcList list = (ProcList)malloc(sizeof(struct _procedure_list));
	list->proc = p;
	list->next = pl;
	return list;
}

// Declarations

Decl astDeclMakeConstDef(char* id, int n){
	Decl ret = (Decl)malloc(sizeof(struct _declaration));
	ret->tag = CONST_DEF;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->constDef.ident = s;
	ret->constDef.num = n;
	return ret;
}

Decl astDeclMakeVarDef(char* id, Decl t){
	Decl ret = (Decl)malloc(sizeof(struct _declaration));
	ret->tag = VAR_DEF;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->varDef.ident = s;
	ret->varDef.type = t;
	return ret;
}

Decl astDeclMakeBasicType(char* tl){
	Decl ret = (Decl)malloc(sizeof(struct _declaration));
	ret->tag = BASIC_TYPE;
	char *s = malloc(strlen(tl)+1);
	strcpy(s, tl);
	ret->basicType.typeLiteral = s;
	return ret;
}

Decl astDeclMakeArrayType(Decl c1, Decl c2, Decl t){
	Decl ret = (Decl)malloc(sizeof(struct _declaration));
	ret->tag = ARRAY_TYPE;
	ret->arrayType.const1 = c1;
	ret->arrayType.const2 = c2;
	ret->arrayType.type = t;
	return ret;
}

Decl astDeclMakeConstNum(int n){
	Decl ret = (Decl)malloc(sizeof(struct _declaration));
	ret->tag = CONST_NUM;
	ret->constNum.num = n;
	return ret;
}

Decl astDeclMakeConstIdent(char* id){
	Decl ret = (Decl)malloc(sizeof(struct _declaration));
	ret->tag = CONST_IDENT;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->constIdent.ident = s;
	return ret;
}

Decl astDeclMakeConstDefSeq(DeclList dl){
	Decl ret = (Decl)malloc(sizeof(struct _declaration));
	ret->tag = CONST_DECL;
	ret->constDecl.list = dl;
	return ret;
}

Decl astDeclMakeVarDefSeq(DeclList dl){
	Decl ret = (Decl)malloc(sizeof(struct _declaration));
	ret->tag = VAR_DECL;
	ret->constDecl.list = dl;
	return ret;
}

DeclList astDeclList(Decl d, DeclList dl){
	DeclList list = (DeclList)malloc(sizeof(struct _declaration_list));
	list->decl = d;
	list->next = dl;
	return list;
}

// Statements

Stm astStmMakeAssign(Exp vA, Exp e){

	Stm ret = (Stm)malloc(sizeof(struct _statement));
	ret->tag = ASSIGN;
	ret->assignStm.varA = vA;
	ret->assignStm.exp = e;
	return ret;
}

Stm astStmMakeIf(Exp i, Stm t, Stm e){

	// Type Checking
	if( i != NULL && (i->tag != BOOL && i->tag != BINOP_COMP && i->tag != BINOP_BOOL && i->tag != VAR_ACCESS && i->tag != FUN_ACCESS) ){ yyerror("type operation error in if condition"); }

	Stm ret = (Stm)malloc(sizeof(struct _statement));
	ret->tag = IF;
	ret->ifStm.exp = i;
	ret->ifStm.thenStm = t;
	ret->ifStm.elseStm = e;
	return ret;
}

Stm astStmMakeWhile(Exp e, Stm d){

	// Type Checking
	if( e != NULL && (e->tag != BOOL && e->tag != BINOP_COMP && e->tag != BINOP_BOOL && e->tag != VAR_ACCESS && e->tag != FUN_ACCESS ) ){ yyerror("type operation error in while loop"); }

	Stm ret = (Stm)malloc(sizeof(struct _statement));
	ret->tag = WHILE;
	ret->whileStm.exp = e;
	ret->whileStm.doStm = d;
	return ret;
}

Stm astStmMakeFor(char* id, Exp ef, Exp et, Stm d){

	// Type Checking
	if( ef != NULL && (ef->tag != NUM && ef->tag != BINOP_INT && ef->tag != VAR_ACCESS && ef->tag != FUN_ACCESS ) ){ yyerror("type operation error in for loop"); }
	if( et != NULL && (et->tag != NUM && et->tag != BINOP_INT && et->tag != VAR_ACCESS && et->tag != FUN_ACCESS ) ){ yyerror("type operation error in for loop"); }

	Stm ret = (Stm)malloc(sizeof(struct _statement));
	ret->tag = FOR;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->forStm.ident = s;
	ret->forStm.expFrom = ef;
	ret->forStm.expTo = et;
	ret->forStm.doStm = d;
	return ret;
}

Stm astStmMakeBreak(char* str){
	Stm ret = (Stm)malloc(sizeof(struct _statement));
	ret->tag = BREAK;
	char *s = malloc(strlen(str)+1);
	strcpy(s, str);
	ret->breakStm.breakStr = s;
	return ret;
}

Stm astStmMakeProc(char* id, ExpList el){
	Stm ret = (Stm)malloc(sizeof(struct _statement));
	ret->tag = PROC_STM;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->procStm.ident = s;
	ret->procStm.list = el;
	return ret;
}

Stm astStmMakeCompound(StmList l){
	Stm ret = (Stm)malloc(sizeof(struct _statement));
	ret->tag = COMPOUND;
	ret->compoundStm.list = l;
	return ret;
}

StmList astStmList(Stm s, StmList l){
	StmList list = (StmList)malloc(sizeof(struct _statement_list));
	list->stm = s;
	list->next = l;
	return list;
}

// Expressions


Exp astExpMakeIdExpList(char* id, ExpList l){
	Exp ret = (Exp)malloc(sizeof(struct _expression));
	ret->tag = FUN_ACCESS;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->expList.ident = s;
	ret->expList.list = l;
	return ret;
}

Exp astExpMakeVarAccess(char* id, Exp e){

	// Type Checking
	if(NULL != e && (e->tag == STR || e->tag == BOOL || e->tag == BINOP_BOOL || e->tag == BINOP_COMP)){yyerror("type access error");}

	Exp ret = (Exp)malloc(sizeof(struct _expression));
	ret->tag = VAR_ACCESS;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->varAccess.ident = s;
	//ret->varAccess.access = e;
	return ret;
}

Exp astExpMakeString(char *string){
	Exp ret = (Exp)malloc(sizeof(struct _expression));
	ret->tag = STR;
	char *s = malloc(strlen(string)+1);
	strcpy(s, string);
	ret->str = s;
	return ret;
}

Exp astExpMakeNum(int n){
	Exp ret = (Exp)malloc(sizeof(struct _expression));
	ret->tag = NUM;
	ret->num = n;
	return ret;
}

Exp astExpMakeBinopInt(BinopInt op, Exp e1, Exp e2){

	// Type Checking
	if( e1 != NULL && (e1->tag != NUM && e1->tag != BINOP_INT && e1->tag != VAR_ACCESS && e1->tag != FUN_ACCESS ) ){ yyerror("type operation error"); }
	if( e2 != NULL && (e2->tag != NUM && e2->tag != BINOP_INT && e2->tag != VAR_ACCESS && e2->tag != FUN_ACCESS ) ){ yyerror("type operation error"); }

	Exp ret = (Exp)malloc(sizeof(struct _expression));
	ret->tag = BINOP_INT;
	ret->binopInt.op = op;
	ret->binopInt.left = e1;
	ret->binopInt.right = e2;
	return ret;
}

Exp astExpMakeBinopComp(BinopComp op, Exp e1, Exp e2){

	// Type Checking
	if( e1 != NULL && (e1->tag != NUM && e1->tag != BINOP_INT && e1->tag != VAR_ACCESS && e1->tag != FUN_ACCESS ) ){ yyerror("type operation error"); }
	if( e2 != NULL && (e2->tag != NUM && e2->tag != BINOP_INT && e2->tag != VAR_ACCESS && e2->tag != FUN_ACCESS ) ){ yyerror("type operation error"); }


	Exp ret = (Exp)malloc(sizeof(struct _expression));
	ret->tag = BINOP_COMP;
	ret->binopComp.op = op;
	ret->binopComp.left = e1;
	ret->binopComp.right = e2;
	return ret;
}



Exp astExpMakeBool(char* value){
	Exp ret = (Exp)malloc(sizeof(struct _expression));
	ret->tag = BOOL;
	char *v = malloc(strlen(value)+1);
	strcpy(v, value);
	ret->boolVal = v;
	return ret;
}

Exp astExpMakeBinopBool(BinopBool op, Exp e1, Exp e2){

	// Type Checking
	if( e1 != NULL && (e1->tag != BOOL && e1->tag != BINOP_COMP && e1->tag != BINOP_BOOL && e1->tag != VAR_ACCESS && e1->tag != FUN_ACCESS ) ){ yyerror("type operation error"); }
	if( e2 != NULL && (e2->tag != BOOL && e2->tag != BINOP_COMP && e2->tag != BINOP_BOOL && e2->tag != VAR_ACCESS && e2->tag != FUN_ACCESS ) ){ yyerror("type operation error"); }


	Exp ret = (Exp)malloc(sizeof(struct _expression));
	ret->tag = BINOP_BOOL;
	ret->binopBool.op = op;
	ret->binopBool.left = e1;
	ret->binopBool.right = e2;
	return ret;
}

ExpList astExpList(Exp e, ExpList l){
	ExpList list = (ExpList)malloc(sizeof(struct _expression_list));
	list->exp = e;
	list->next = l;
	return list;
}

// -- Print functions -- //

void printIntOp(BinopInt op){
	switch(op){
		case BINOP_PLUS:
			printf("+");
			break;
		case BINOP_MINUS:
			printf("-");
			break;
		case BINOP_MULT:
			printf("*");
			break;
		case BINOP_DIV:
			printf(" div ");
			break;
		case BINOP_MOD:
			printf(" mod ");
			break;
		default:
			yyerror("Error printing operator");
	}
}

void printCompOp(BinopComp op){
	switch(op){
		case BINOP_EQ:
			printf(" = ");
			break;
		case BINOP_NOTEQ:
			printf(" <> ");
			break;
		case BINOP_LT:
			printf(" < ");
			break;
		case BINOP_LEQT:
			printf(" <= ");
			break;
		case BINOP_GT:
			printf(" > ");
			break;
		case BINOP_GEQT:
			printf(" >= ");
			break;
		default:
			yyerror("Error printing operator");
	}
}

void printBoolOp(BinopInt op){
	switch(op){
		case BINOP_AND:
			printf(" and ");
			break;
		case BINOP_OR:
			printf(" or ");
			break;
		case UNOP_NOT:
			printf(" not ");
			break;
		default:
			yyerror("Error printing operator");
	}
}

void printExp(Exp e){
	if(!e){ return; }
	switch(e->tag){
		case NUM:
			printf("%d", e->num);
			break;
		case STR:
			printf("%s", e->str);
			break;
		case BOOL:
			printf("%s", e->boolVal);
			break;
		case FUN_ACCESS:
			printf("%s", e->expList.ident);
			printf("(");
			ExpList cur = e->expList.list;
			if(cur != NULL){
				while(cur->next != NULL){
					printExp(cur->exp);
					cur = cur->next;
					printf(",");
				}
				printExp(cur->exp);
			}
			printf(")");
			break;
		case VAR_ACCESS:
			printf("%s", e->varAccess.ident);
			if(e->varAccess.access){
				printf("[");
				printExp(e->varAccess.access);
				printf("]");
			}
			break;
		case BINOP_INT:
			printf("{");
			printExp(e->binopInt.left);
			printIntOp(e->binopInt.op);
			printExp(e->binopInt.right);
			printf("}");
			break;
		case BINOP_COMP:
			printf("{");
			printExp(e->binopComp.left);
			printCompOp(e->binopComp.op);
			printExp(e->binopComp.right);
			printf("}");
			break;
		case BINOP_BOOL:
			printf("{");
			printExp(e->binopBool.left);
			printBoolOp(e->binopBool.op);
			printExp(e->binopBool.right);
			printf("}");
			break;
		default:
			yyerror("Error printing experssion");
	}
}

void printStm(Stm s){
	if(!s){ return; }
	switch(s->tag){
		case ASSIGN:
			printExp(s->assignStm.varA);
			printf(" := ");
			printExp(s->assignStm.exp);
			break;
		case IF:
			printf("{");
			printf("if "); printExp(s->ifStm.exp);
			printf(" then "); printStm(s->ifStm.thenStm);
			if(s->ifStm.elseStm){
				printf(" else "); printStm(s->ifStm.elseStm);
			}
			printf("}");
			break;
		case WHILE:
			printf("{");
			printf("while "); printExp(s->whileStm.exp);
			printf(" do "); printStm(s->whileStm.doStm);
			printf("}");
			break;
		case FOR:
			printf("{");
			printf("for "); printf("%s", s->forStm.ident); printf(" := "); printExp(s->forStm.expFrom);
			printf(" to "); printExp(s->forStm.expTo);
			printf(" do "); printStm(s->forStm.doStm);
			printf("}");
			break;
		case BREAK:
			printf(" %s ", s->breakStm.breakStr);
			break;
		case PROC_STM:
			printf("%s", s->procStm.ident);
			printf("(");
			ExpList curE = s->procStm.list;
			if(curE != NULL){
				while(curE->next != NULL){
					printExp(curE->exp);
					curE = curE->next;
					printf(",");
				}
				printExp(curE->exp);
			}
			printf(")");
			break;
		case COMPOUND:
			printf("begin ");
			StmList curS = s->compoundStm.list;
			if(curS != NULL){
				while(curS->next != NULL){
					printStm(curS->stm);
					curS = curS->next;
					printf("; ");
				}
				printStm(curS->stm);
			}
			printf(" end");
			break;
		default:
			yyerror("Error printing statement");
	}
}

void printDecl(Decl d){
	if(!d){ return; }
	switch(d->tag){
		case CONST_DEF:
			printf("%s", d->constDef.ident);
			printf(" = ");
			printf("%d", d->constDef.num);
			printf("; ");
			break;
		case VAR_DEF:
			printf("%s", d->varDef.ident);
			printf(" : ");
			printDecl(d->varDef.type);
			printf("; ");
			break;
		case BASIC_TYPE:
			printf("%s", d->basicType.typeLiteral);
			break;
		case ARRAY_TYPE:
			printf("array");
			printf("[");
			printDecl(d->arrayType.const1);
			printf(" .. ");
			printDecl(d->arrayType.const2);
			printf("]");
			printf(" of ");
			printDecl(d->arrayType.type);
			break;
		case CONST_NUM:
			printf("%d", d->constNum.num);
			break;
		case CONST_IDENT:
			printf("%s", d->constIdent.ident);
			break;
		case CONST_DECL:
			printf("const ");
			DeclList curC = d->constDecl.list;
			if(curC != NULL){
				while(curC->next != NULL){
					printDecl(curC->decl);
					curC = curC->next;
				}
				printDecl(curC->decl);
			}
			printf("\n");
			break;
		case VAR_DECL:
			printf("var ");
			DeclList curV = d->varDecl.list;
			if(curV != NULL){
				while(curV->next != NULL){
					printDecl(curV->decl);
					curV = curV->next;
				}
				printDecl(curV->decl);
			}
			printf("\n");
			break;
		default:
			yyerror("Error printing declaration");
	}
}

void printProc(Proc p){
	if(!p){ return; }
	switch(p->tag){
		case PROC:
			printProc(p->proc.header);
			printProc(p->proc.body);
			printf("; ");
			printf("\n");
			break;
		case PROC_HEADER:
			printf("procedure ");
			printf("%s", p->procHeader.ident);
			printf("(");
			ProcList curP = p->procHeader.list;
			if(curP != NULL){
				while(curP->next != NULL){
					printProc(curP->proc);
					printf("; ");
					curP = curP->next;
				}
				printProc(curP->proc);
			}
			printf(")");
			printf(";");
			printf("\n");
			break;
		case FUN_HEADER:
			printf("function ");
			printf("%s", p->funHeader.ident);
			printf("(");
			ProcList curF = p->funHeader.list;
			if(curF != NULL){
				while(curF->next != NULL){
					printProc(curF->proc);
					printf(", ");
					curF = curF->next;
				}
				printProc(curF->proc);
			}
			printf(")");
			printf(" : ");
			printDecl(p->funHeader.type);
			printf("; ");
			printf("\n");
			break;
		case PROC_BODY:
			printDecl(p->procBody.decl);
			printStm(p->procBody.stm);
			break;
		case PARAM:
			printf("%s", p->param.ident);
			printf(" : ");
			printDecl(p->param.type);
			break;
		default:
			yyerror("Error printing procedure or function");
	}
}

void printProgram(Program prog){
	if(!prog){ return; }
	switch(prog->tag){
		case PROGRAM:
			printProgram(prog->program.header);
			printProgram(prog->program.body);
			printf(".");
			break;
		case PROGRAM_HEADER:
			printf("program ");
			printf("%s", prog->programHeader.ident);
			printf(";");
			printf("\n");
			break;
		case PROGRAM_BODY:
			printDecl(prog->programBody.constDecl);
			ProcList curP = prog->programBody.list;
			if(curP != NULL){
				while(curP->next != NULL){
					printProc(curP->proc);
					curP = curP->next;
				}
				printProc(curP->proc);
			}
			printDecl(prog->programBody.varDecl);
			printStm(prog->programBody.stm);
			break;
		default:
			yyerror("Error printing program");
	}
}
