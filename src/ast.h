#ifndef _AST_H_
#define _AST_H_


// -- Expression Structs -- //

typedef enum {BINOP_PLUS, BINOP_MINUS, BINOP_MULT, BINOP_DIV, BINOP_MOD} BinopInt;
typedef enum {BINOP_EQ, BINOP_NOTEQ, BINOP_LT, BINOP_LEQT, BINOP_GT, BINOP_GEQT} BinopComp;
typedef enum {BINOP_AND, BINOP_OR, UNOP_NOT} BinopBool;

typedef struct _expression {
	enum {NUM, STR, BOOL, VAR_ACCESS, BINOP_INT, BINOP_COMP, BINOP_BOOL, FUN_ACCESS} tag;
	union{
		char *str;     // case STR
		int num; 	   // case NUM
		char *boolVal; // case BOOL
		struct{		   // case VAR_ACCESS
			char* ident;
			struct _expression *access;
		} varAccess;
		struct{		   // case BINOP_INT
			BinopInt op;
			struct _expression *left, *right;
		} binopInt;
		struct{		   // case BINOP_COMP
			BinopComp op;
			struct _expression *left, *right;
		} binopComp;
		struct{        // case BINOP_BOOL
			BinopBool op;
			struct _expression *left, *right;
		} binopBool;
		struct{        // case FUN_ACCESS
			char* ident;
			struct _expression_list *list;
		} expList;
	};
}*Exp;

typedef struct _expression_list{
	Exp exp;
	struct _expression_list* next;
}*ExpList;

// -- Statement Structs -- //

typedef struct _statement {
	enum {ASSIGN, IF, WHILE, FOR, BREAK, PROC_STM, COMPOUND} tag;
	union{
		struct{ // case ASSIGN
			Exp varA;
			Exp exp;
		}assignStm;
		struct{ // case IF
			Exp exp;
			struct _statement *thenStm;
			struct _statement *elseStm;
		}ifStm;
		struct{ // case WHILE
			Exp exp;
			struct _statement *doStm;
		}whileStm;
		struct{ // case FOR
			char* ident;
			Exp expFrom;
			Exp expTo;
			struct _statement *doStm;
		}forStm;
		struct{ // case BREAK
			char* breakStr;
		}breakStm;
		struct{ // case PROC_STM
			char* ident;
			struct _expression_list *list;
		}procStm;
		struct{ // case COMPOUND
			struct _statement_list *list;
		}compoundStm;
	};
}*Stm;

typedef struct _statement_list{
	Stm stm;
	struct _statement_list* next;
}*StmList;

// -- Declaration Structs -- //

typedef struct _declaration{
	enum { CONST_DEF, VAR_DEF, BASIC_TYPE, ARRAY_TYPE, CONST_NUM, CONST_IDENT, CONST_DECL, VAR_DECL} tag;
	union{
		struct{ // case CONST_DEF
			char* ident;
			int num;
		}constDef;
		struct{ // case VAR_DEF
			char* ident;
			struct _declaration *type;
		}varDef;
		struct{ // case BASIC_TYPE
			char* typeLiteral;
		}basicType;
		struct{ // case ARRAY_TYPE
			struct _declaration *const1;
			struct _declaration *const2;
			struct _declaration *type;
		}arrayType;
		struct{ // case CONST_NUM
			int num;
		}constNum;
		struct{ // case CONST_IDENT
			char* ident;
		}constIdent;
		struct{ // case CONST_DECL
			struct _declaration_list *list;
		}constDecl;
		struct{ // case VAR_DECL
			struct _declaration_list *list;
		}varDecl;
	};
}*Decl;

typedef struct _declaration_list{
	Decl decl;
	struct _declaration_list* next;
}*DeclList;

// -- 'Procedures and Functions' Structs -- //

typedef struct _procedure{
	enum {PROC, PROC_HEADER, FUN_HEADER, PROC_BODY, PARAM} tag;
	union{
		struct{ // case PROC
			struct _procedure *header;
			struct _procedure *body;
		}proc;
		struct{ // case PROC_HEADER
			char* ident;
			struct _procedure_list *list;
		}procHeader;
		struct{ // case FUN_HEADEER
			char* ident;
			struct _procedure_list *list;
			Decl type;
		}funHeader;
		struct{ // case PROC_BODY
			Decl decl;
			Stm stm;
		}procBody;
		struct{ // case PARAM
			char* ident;
			Decl type;
		}param;
	};
}*Proc;

typedef struct _procedure_list{
	Proc proc;
	struct _procedure_list* next;
}*ProcList;

// -- Program Structs -- //

typedef struct _program{
	enum{PROGRAM, PROGRAM_HEADER, PROGRAM_BODY}tag;
	union{
		struct{ // case Program
			struct _program *header;
			struct _program *body;
		}program;
		struct{ // case PROGRAM_HEADER
			char* ident;
		}programHeader;
		struct{ // case PROGRAM_BODY
			Decl constDecl;
			ProcList list;
			Decl varDecl;
			Stm stm;
		}programBody;
	};
}*Program;

// -- Node construction -- //

// Program
Program astProgramMakeProgram(Program,Program);
Program astProgramMakeHeader(char*);
Program astProgramMakeBody(Decl, ProcList, Decl, Stm);

// Procedures and Functions
Proc astProcMakeProc(Proc, Proc);
Proc astProcMakeProcHeader(char*, ProcList);
Proc astProcMakeFunHeader(char*, ProcList, Decl);
Proc astProcMakeProcBody(Decl, Stm);
Proc astProcMakeParam(char*, Decl);
ProcList astProcList(Proc, ProcList);

// Declarations
Decl astDeclMakeConstDef(char*, int);
Decl astDeclMakeVarDef(char*, Decl);
Decl astDeclMakeBasicType(char*);
Decl astDeclMakeArrayType(Decl, Decl, Decl);
Decl astDeclMakeConstNum(int);
Decl astDeclMakeConstIdent(char*);
Decl astDeclMakeConstDefSeq(DeclList);
Decl astDeclMakeVarDefSeq(DeclList);
DeclList astDeclList(Decl, DeclList);

// Statements
Stm astStmMakeAssign(Exp,Exp);
Stm astStmMakeIf(Exp,Stm,Stm);
Stm astStmMakeWhile(Exp, Stm);
Stm astStmMakeFor(char*,Exp,Exp,Stm);
Stm astStmMakeBreak(char*);
Stm astStmMakeProc(char*,ExpList);
Stm astStmMakeCompound(StmList);
StmList astStmList(Stm,StmList);

// Expressions
Exp astExpMakeIdExpList(char*, ExpList);
Exp astExpMakeId(char*);
Exp astExpMakeString(char*);
Exp astExpMakeNum(int);
Exp astExpMakeBool(char*);
Exp astExpMakeBinopInt(BinopInt, Exp, Exp);
Exp astExpMakeBinopComp(BinopComp, Exp, Exp);
Exp astExpMakeBinopBool(BinopBool, Exp, Exp);
Exp astExpMakeVarAccess(char*, Exp);
ExpList astExpList(Exp,ExpList);

// Print functions
void printOp(BinopInt);
void printExp(Exp);
void printStm(Stm);
void printDecl(Decl);
void printProc(Proc);
void printProgram(Program);

#endif
