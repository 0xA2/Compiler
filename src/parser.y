%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "symTable.h"

int yylex (void);
void yyerror (char const *);


Program root;
symTable table;

int inFun = 0;
%}

%code requires {
	#include "ast.h"
	#include "symTable.h"
}


%nonassoc 
TOK_PROGRAM
TOK_FUNCTION
TOK_PROCEDURE
TOK_CONST
TOK_VAR
TOK_BEGIN
TOK_END
TOK_IF
TOK_WHILE
TOK_DO
TOK_FOR
TOK_TO
TOK_BOOL
TOK_TYPE_INT
TOK_TYPE_BOOL
TOK_TYPE_STRING
TOK_ARR
TOK_OF
TOK_BREAK
TOK_COMMA
TOK_PERIOD
TOK_COLON
TOK_SEMICOLON
TOK_LPAREN
TOK_RPAREN
TOK_LBRAC
TOK_RBRAC
TOK_ID 
TOK_NUM 
TOK_STRING 
TOK_ASSIGN

%nonassoc TOK_THEN
%nonassoc TOK_ELSE

%nonassoc TOK_BINOP_EQ TOK_BINOP_NOTEQ TOK_BINOP_LT TOK_BINOP_LEQT TOK_BINOP_GT TOK_BINOP_GEQT
%left TOK_BINOP_PLUS TOK_BINOP_MINUS TOK_BINOP_OR
%left TOK_BINOP_MULT TOK_BINOP_DIV TOK_BINOP_MOD TOK_BINOP_AND
%left TOK_UNOP_NOT TOK_UNOP_MINUS


%union{
	char* typeStr;
	int typeInt;
	Exp typeExp;
	ExpList typeExpList;
	Stm typeStm;
	StmList typeStmList;
	Decl typeDecl;
	DeclList typeDeclList;
	Proc typeProc;
	ProcList typeProcList;
	Program typeProgram;
}

%type <typeProgram> program
%type <typeProgram> programHeader
%type <typeProgram> programBody
%type <typeProcList> procDecl
%type <typeProc> proc
%type <typeProc> procHeader
%type <typeProc> procBody
%type <typeProc> param
%type <typeProcList> paramList 
%type <typeProcList> paramList1
%type <typeDecl> constDecl
%type <typeDeclList> constDefSeq
%type <typeDecl> varDecl
%type <typeDeclList> varDefSeq
%type <typeDecl> constDef
%type <typeDecl> varDef
%type <typeDecl> type
%type <typeDecl> basicType
%type <typeDecl> arrayType
%type <typeDecl> constant
%type <typeStm> stm
%type <typeStm> assignStm
%type <typeStm> ifStm
%type <typeStm> whileStm
%type <typeStm> forStm
%type <typeStm> break
%type <typeStm> procStm
%type <typeStm> compoundStm
%type <typeStmList> stmList
%type <typeExp> exp 
%type <typeExp> varAccess
%type <typeExpList> expList
%type <typeExpList> expList1
%type <typeInt> TOK_NUM
%type <typeStr> TOK_STRING
%type <typeStr> TOK_ID
%type <typeStr> TOK_BOOL
%type <typeStr> TOK_BREAK
%type <typeStr> TOK_TYPE_INT
%type <typeStr> TOK_TYPE_BOOL
%type <typeStr> TOK_TYPE_STRING
%type <typeStr> TOK_CONST

%%

start : program { root = $1;}
      ;

/* Program */

program : programHeader programBody TOK_PERIOD { $$ = astProgramMakeProgram($1,$2); }
		;
		
programHeader : TOK_PROGRAM TOK_ID TOK_SEMICOLON { table = initTable(table); $$ = astProgramMakeHeader($2); }
			  ;

programBody : constDecl procDecl varDecl compoundStm { $$ = astProgramMakeBody($1,$2,$3,$4); popScope(table);}
			;

procDecl : proc procDecl { $$ = astProcList($1, $2); }
		 | %empty        { $$ = NULL;                }




/* Procedures and Functions */

proc : procHeader procBody TOK_SEMICOLON { $$ = astProcMakeProc($1, $2); }
	 ;
	
procHeader : TOK_PROCEDURE TOK_ID TOK_LPAREN paramList TOK_RPAREN TOK_SEMICOLON                    { table = push(table, $2, ID_NULL, ID_PROC, 0); inFun = 1; $$ = astProcMakeProcHeader($2, $4); }
           | TOK_FUNCTION TOK_ID TOK_LPAREN paramList TOK_RPAREN TOK_COLON basicType TOK_SEMICOLON { table = push(table, $2, getType($7), ID_FUN, 0); inFun = 1; $$ = astProcMakeFunHeader($2, $4, $7); }
		   ;

procBody : varDecl compoundStm { $$ = astProcMakeProcBody($1, $2); inFun = 0; }
		 ;

param : TOK_ID TOK_COLON type { table = push(table, $1, getType($3), ID_PARAM, 1); $$ = astProcMakeParam($1, $3); }
	  ;

paramList : paramList1 { $$ = $1;   }
		  | %empty     { $$ = NULL; }
		  ;

paramList1 : param TOK_SEMICOLON paramList1 { $$ = astProcList($1, $3);   }
		   | param                          { $$ = astProcList($1, NULL); }
		   ;





/* Declarations  */

constDecl : TOK_CONST constDefSeq { $$ = astDeclMakeConstDefSeq($2); }
		  | %empty                { $$ = NULL; }
		  ;

constDefSeq : constDef constDefSeq { $$ = astDeclList($1,$2);    }
			| constDef             { $$ = astDeclList($1, NULL); }
			;

varDecl : TOK_VAR varDefSeq { $$ = astDeclMakeVarDefSeq($2); }
		| %empty            { $$ = NULL;                     }
		;

varDefSeq : varDef varDefSeq { $$ = astDeclList($1,$2);   }
		  | varDef           { $$ = astDeclList($1,NULL); }
		  ;

constDef : TOK_ID TOK_BINOP_EQ TOK_NUM TOK_SEMICOLON { table = push(table, $1, ID_NUM, ID_CONST, inFun); $$ = astDeclMakeConstDef($1,$3); }
		 ;

varDef : TOK_ID TOK_COLON type TOK_SEMICOLON { table = push(table, $1, getType($3), getKind($3), inFun); $$ = astDeclMakeVarDef($1, $3); }
	   ;

type : basicType { $$ = $1; }
	 | arrayType { $$ = $1; }
	 ;

basicType : TOK_TYPE_INT    { $$ = astDeclMakeBasicType($1); }
          | TOK_TYPE_BOOL   { $$ = astDeclMakeBasicType($1); }
          | TOK_TYPE_STRING { $$ = astDeclMakeBasicType($1); }
          ;

arrayType : TOK_ARR TOK_LBRAC constant TOK_PERIOD TOK_PERIOD constant TOK_RBRAC TOK_OF basicType { $$ = astDeclMakeArrayType($3,$6,$9); }
          ;

constant : TOK_NUM { $$ = astDeclMakeConstNum($1);   }
		 | TOK_ID  { $$ = astDeclMakeConstIdent($1); }
         ;





/* Statements  */

stm : assignStm   { $$ = $1; }
	| ifStm       { $$ = $1; }
	| whileStm    { $$ = $1; }
	| forStm      { $$ = $1; }
	| break       { $$ = $1; }
	| procStm     { $$ = $1; }
	| compoundStm { $$ = $1; }
	;

assignStm : varAccess TOK_ASSIGN exp { validateAssign(table, $1, $3); $$ = astStmMakeAssign($1, $3); } 
		  ;

ifStm : TOK_IF exp TOK_THEN stm              { validateIf(table, $2); $$ = astStmMakeIf($2,$4,NULL); }
	  | TOK_IF exp TOK_THEN stm TOK_ELSE stm { validateIf(table, $2); $$ = astStmMakeIf($2,$4,$6);   }
	  ;

whileStm : TOK_WHILE exp TOK_DO stm { validateWhile(table,$2); $$ = astStmMakeWhile($2, $4); }
		 ;

forStm : TOK_FOR TOK_ID TOK_ASSIGN exp TOK_TO exp TOK_DO stm { validateFor(table,$2,$4,$6); $$ = astStmMakeFor($2, $4, $6, $8); }
	   ;

break : TOK_BREAK { $$ = astStmMakeBreak($1); }
      ;

procStm : TOK_ID TOK_LPAREN expList TOK_RPAREN { validateVarAccess(table, $1); $$ = astStmMakeProc($1, $3); }
        ;

compoundStm : TOK_BEGIN stmList TOK_END { $$ = astStmMakeCompound($2); }
		    ;

stmList : stm TOK_SEMICOLON stmList { $$ = astStmList($1, $3);   }
		| stm                       { $$ = astStmList($1, NULL); }
		;




/* Expressions  */

exp : TOK_NUM                                  { $$ = astExpMakeNum($1);                                                             }
	| TOK_STRING                               { $$ = astExpMakeString($1);                                                          }
	| TOK_BOOL                                 { $$ = astExpMakeBool($1);                                                            }
    | exp TOK_BINOP_PLUS exp                   { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopInt(BINOP_PLUS, $1, $3);    }
	| exp TOK_BINOP_MULT exp                   { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopInt(BINOP_MULT, $1, $3);    }
	| exp TOK_BINOP_MINUS exp                  { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopInt(BINOP_MINUS, $1, $3);   }
	| exp TOK_BINOP_DIV exp                    { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopInt(BINOP_DIV, $1, $3);     }
	| exp TOK_BINOP_MOD exp                    { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopInt(BINOP_MOD, $1, $3);     }
	| TOK_BINOP_MINUS exp %prec TOK_UNOP_MINUS { validateBinopInt(table, $2, NULL);  $$ = astExpMakeBinopInt(BINOP_MINUS, NULL, $2); }
	| exp TOK_BINOP_EQ exp                     { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopComp(BINOP_EQ, $1, $3);     }
	| exp TOK_BINOP_NOTEQ exp                  { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopComp(BINOP_NOTEQ, $1, $3);  }
	| exp TOK_BINOP_LT exp                     { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopComp(BINOP_LT, $1, $3);     }
	| exp TOK_BINOP_LEQT exp                   { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopComp(BINOP_LEQT, $1, $3);   }
	| exp TOK_BINOP_GT exp                     { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopComp(BINOP_GT, $1, $3);     }
	| exp TOK_BINOP_GEQT exp                   { validateBinopInt(table, $1, $3);    $$ = astExpMakeBinopComp(BINOP_GEQT, $1, $3);   }
	| exp TOK_BINOP_AND exp                    { validateBinopBool(table, $1, $3);   $$ = astExpMakeBinopBool(BINOP_AND, $1, $3);    }
	| exp TOK_BINOP_OR exp                     { validateBinopBool(table, $1, $3);   $$ = astExpMakeBinopBool(BINOP_OR, $1, $3);     }
	| TOK_UNOP_NOT exp                         { validateBinopBool(table, $2, NULL); $$ = astExpMakeBinopBool(UNOP_NOT, NULL, $2);   }
	| TOK_LPAREN exp TOK_RPAREN                { $$ = $2;                                                                            }
	| TOK_ID TOK_LPAREN expList TOK_RPAREN     { validateVarAccess(table, $1); $$ = astExpMakeIdExpList($1, $3);                     }
	| varAccess                                { $$ = $1;                                                                            }
    ;


varAccess : TOK_ID                         { validateVarAccess(table, $1); $$ = astExpMakeVarAccess($1, NULL); }
          | TOK_ID TOK_LBRAC exp TOK_RBRAC { validateVarAccess(table, $1); $$ = astExpMakeVarAccess($1, $3);   }
		  ;

expList: expList1 { $$ = $1;  }
	   | %empty   { $$ = NULL;}
	   ;

expList1: exp TOK_COMMA expList1 { $$ = astExpList($1,$3);  }
	    | exp                    { $$ = astExpList($1,NULL);}
	    ;





%%

/* Error Handlind */

void yyerror (char const *msg) {
  printf("\nParse error: %s\n", msg);
  exit(EXIT_FAILURE);
}

void sterror (char const *msg){
  printf("\nTable access error: %s\n", msg);
  exit(EXIT_FAILURE);
}

void icerror (char const *msg){
  printf("\nIntermediate code error: %s\n", msg);
  exit(EXIT_FAILURE);
}

void tmerror (char const *msg){
  printf("\nTranslation to MIPS error: %s\n", msg);
  exit(EXIT_FAILURE);

}

/* Print Functions */
void printASTAux(){ printProgram(root); }
void printTableAux(){ printTable(table); }

/* Getters */
Program getAST(){ return root; }
