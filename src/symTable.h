#ifndef _SYM_TABLE_H_
#define _SYM_TABLE_H_

#include "ast.h"

// -- Lookup Stack Struct -- //
typedef struct _lookupstack{
	char* ident;
	enum {ID_NUM,ID_STR,ID_BOOL,ID_NULL} Type;
	enum {ID_VAR,ID_CONST,ID_ARR,ID_FUN,ID_PROC,ID_PARAM,ID_TMP} Kind;
	int inScope;
	struct _lookupstack *bottom;
}*symTable;

// Stack Functions
symTable initTable();
symTable push(symTable,char*,int,int,int);
symTable pop(symTable);
symTable lookup(symTable,char*);
void popScope(symTable);
int getType(Decl);
int getKind(Decl);
int getTypeFromIdent(char*);
void validateVarAccess(symTable, char*);
void validateBinopInt(symTable, Exp, Exp);
void validateBinopBool(symTable, Exp, Exp);
void validateAssign(symTable, Exp, Exp);
void validateIf(symTable, Exp);
void validateWhile(symTable, Exp);
void validateFor(symTable,char*,Exp,Exp);
void verifyInLoop(int);
void printTable(symTable);



#endif
