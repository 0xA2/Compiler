#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symTable.h"
#include "parser.tab.h"

extern void yyerror();
extern void sterror();

symTable initTable(symTable table){

	// Add I/0 functions to the table
	char* readInt = malloc(strlen("readint")+1); strcpy(readInt,"readint");
	char* writeInt = malloc(strlen("writeint")+1); strcpy(writeInt,"writeint");
	char* writeStr = malloc(strlen("writestr")+1); strcpy(writeStr,"writestr");
	table = push(table, readInt, ID_NUM, ID_FUN, 0);
	table = push(table, writeInt, ID_NULL, ID_FUN, 0);
	table = push(table, writeStr, ID_NULL, ID_FUN, 0);
	return table;

}

symTable push(symTable table, char* id, int t, int k, int f){
	symTable ret = (symTable)malloc(sizeof(struct _lookupstack));
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->ident = s;
	ret->Type = t;
	ret->Kind = k;
	ret->inScope = f;
	ret->bottom = table;
	return ret;
}

symTable pop(symTable table){
	if(NULL == table){ sterror("Trying to pop an empty stack"); }
	symTable tmp = (symTable)malloc(sizeof(struct _lookupstack));
	tmp = table;
	table = table->bottom;
	free(tmp);
	return table;
}

symTable lookup(symTable table, char* id){

	if(NULL == table){ return NULL; }
	symTable cur = (symTable)malloc(sizeof(struct _lookupstack));
	cur = table;
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	while(NULL != cur){
		if(strcmp(cur->ident,s) == 0){
			return cur;
		}
		cur = cur->bottom;
	}
	return NULL;
}

void popScope(symTable table){
	if(NULL == table){ sterror("Trying to delete scope from an empty table"); }
	symTable cur = (symTable)malloc(sizeof(struct _lookupstack));
	symTable next = (symTable)malloc(sizeof(struct _lookupstack));
	cur = table;
	next = table->bottom;
	if(cur->inScope){ pop(table); if(NULL == next){return;} }
	while(NULL != next){
		if(next->inScope){
			symTable tmp = (symTable)malloc(sizeof(struct _lookupstack));
			tmp = next;
			next = next->bottom;
			cur->bottom = next;
			free(tmp);
		}
		else{
			cur = cur->bottom;
			next = next->bottom;
		}
	}
}

int getType(Decl t){
	if(!t){ sterror("Trying to get type from null"); }
	switch(t->tag){
		case BASIC_TYPE:
			if( strcmp(t->basicType.typeLiteral, "integer") == 0){ return ID_NUM; }
			else if( strcmp(t->basicType.typeLiteral, "string") == 0){ return ID_STR; }
			else if( strcmp(t->basicType.typeLiteral, "boolean") == 0){ return ID_BOOL; }
			else{ sterror("Invalid declaration type "); }
			break;
		case ARRAY_TYPE:
			if( strcmp(t->arrayType.type->basicType.typeLiteral, "integer") == 0){ return ID_NUM; }
			else if( strcmp(t->arrayType.type->basicType.typeLiteral, "string") == 0){ return ID_STR; }
			else if( strcmp(t->arrayType.type->basicType.typeLiteral, "boolean") == 0){ return ID_BOOL; }
			else{ sterror("Invalid declaration type "); }
			break;
		default:
			sterror("Trying to get type from non-type declaration");
	}
}

int getKind(Decl t){
	if(!t){ sterror("Trying to get variable kind from null"); }
	switch(t->tag){
		case BASIC_TYPE:
			return ID_VAR;
		case ARRAY_TYPE:
			return ID_ARR;
		default:
			sterror("Variable with unrecognized type");
	}
}

void validateVarAccess(symTable table, char* id){
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	symTable cur = lookup(table, id);
	if(NULL == cur){ sterror("Trying to access undeclared variable"); }
	return;
}

void validateBinopInt(symTable table, Exp e1, Exp e2){
	if(NULL != e1){
		switch(e1->tag){
			case VAR_ACCESS:{
				symTable cur = lookup(table, e1->varAccess.ident);
				if(cur->Type != ID_NUM){ yyerror("type operation error"); }
				break;
			}
			case FUN_ACCESS:{
				symTable cur = lookup(table, e1->expList.ident);
				if(cur->Type != ID_NUM){ yyerror("type operation error"); }
				break;
			}
		}
	}
	if(NULL != e2){
		switch(e2->tag){
			case VAR_ACCESS:{
				symTable cur = lookup(table, e2->varAccess.ident);
				if(cur->Type != ID_NUM){ yyerror("type operation error"); }
				break;
			}
			case FUN_ACCESS:{
				symTable cur = lookup(table, e2->expList.ident);
				if(cur->Type != ID_NUM){ yyerror("type operation error"); }
				break;
			}
		}
	}
}

void validateBinopBool(symTable table, Exp e1, Exp e2){
	if(NULL != e1){
		switch(e1->tag){
			case VAR_ACCESS:{
				symTable cur = lookup(table, e1->varAccess.ident);
				if(cur->Type != ID_BOOL){ yyerror("type operation error"); }
				break;
			}
			case FUN_ACCESS:{
				symTable cur = lookup(table, e1->expList.ident);
				if(cur->Type != ID_BOOL){ yyerror("type operation error"); }
				break;
			}
		}
	}
	if(NULL != e2){
		switch(e2->tag){
			case VAR_ACCESS:{
				symTable cur = lookup(table, e2->varAccess.ident);
				if(cur->Type != ID_BOOL){ yyerror("type operation error"); }
				break;
			}
			case FUN_ACCESS:{
				symTable cur = lookup(table, e2->expList.ident);
				if(cur->Type != ID_BOOL){ yyerror("type operation error"); }
				break;
			}
		}
	}
}

void validateAssign(symTable table, Exp var, Exp content){
	validateVarAccess(table, var->varAccess.ident);
	symTable cur = lookup(table, var->varAccess.ident);
	if(ID_CONST == cur->Kind){ yyerror("trying to assign value to a constant variable"); }
	switch(content->tag){
		case NUM:
			if(cur->Type != ID_NUM){ yyerror("type operation error"); }
			break;
		case STR:
			if(cur->Type != ID_STR){ yyerror("type operation error"); }
			break;
		case BOOL:
			if(cur->Type != ID_BOOL){ yyerror("type operation error"); }
			break;
		case BINOP_INT:
			if(cur->Type != ID_NUM){ yyerror("type operation error"); }
			break;
		case BINOP_COMP:
			if(cur->Type != ID_BOOL){ yyerror("type operation error"); }
			break;
		case BINOP_BOOL:
			if(cur->Type != ID_BOOL){ yyerror("type operation error"); }
			break;
		case VAR_ACCESS:{
			validateVarAccess(table, content->varAccess.ident);
			symTable tmp = lookup(table, content->varAccess.ident);
			if(cur->Type != tmp->Type){ yyerror("type operation error"); }
			break;
		}
		case FUN_ACCESS:{
			symTable tmp = lookup(table, content->expList.ident);
			if(cur->Type != tmp->Type){ yyerror("type operation error"); }
			break;
		}
		default:
			yyerror("invalid variable tag");
	}
}

void validateIf(symTable table, Exp cond){
	if(NULL != cond){
		switch(cond->tag){
			case VAR_ACCESS:{
				validateVarAccess(table, cond->varAccess.ident);
				symTable cur = lookup(table, cond->varAccess.ident);
				if(cur->Type != ID_BOOL){ yyerror("type operation error"); }
				break;
			}
			case FUN_ACCESS:{
				validateVarAccess(table, cond->expList.ident);
				symTable cur = lookup(table, cond->expList.ident);
				if(cur->Type != ID_BOOL){ yyerror("type operation error"); }
				break;
			}
		}
	}
}

void validateWhile(symTable table, Exp cond){
	validateIf(table, cond);
}

void validateFor(symTable table, char* id, Exp from, Exp to){
	validateVarAccess(table, id);
	validateBinopInt(table,from,to);
}

void printTable(symTable table){
	symTable cur = (symTable)malloc(sizeof(struct _lookupstack));
	cur = table;
	while(NULL != cur){
		printf("\nIdentifier: %s\nType: %d\nKind: %d\nScope identifier: %d\n", cur->ident, cur->Type, cur->Kind, cur->inScope);
		cur = cur->bottom;
	}
}
