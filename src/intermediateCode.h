#ifndef _INTERMEDIATE_CODE_H_
#define _INTERMEDIATE_CODE_H_

#include "ast.h"
#include "tmpTable.h"

typedef enum{OPCODE_DEF, OPCODE_CALL, OPCODE_RETURN, OPCODE_LABEL, OPCODE_JUMP, OPCODE_MOVE, OPCODE_MOVEI, OPCODE_MOVES, OPCODE_MOVEB, OPCODE_ADD, OPCODE_SUB, OPCODE_MULT, OPCODE_DIV, OPCODE_MOD, OPCODE_EQ, OPCODE_NOTEQ, OPCODE_LT, OPCODE_LEQT, OPCODE_GT, OPCODE_GEQT} Opcode;
typedef int Addr;

typedef struct _paramlist{
	int value;
	struct _paramlist *next;
}*ParamList;

typedef struct _instr{
	Opcode opcode;
	Addr arg1, arg2, arg3, arg4;
	char* ident;
	ParamList list;
	char* strLiteral;

}*Instr;

typedef struct _instrlist{
	Instr in;
	struct _instrlist *next;
}*InstrList;

typedef struct _datalist{
	char* id;
	int tmpValue;
	char* string;
	struct _datalist *next;
}*DataList;

// Instruction Functions
InstrList addInstr(InstrList, Instr);
ParamList addParam(ParamList,int);
DataList addStr(DataList,char*,char*);
void emit1(int,int);
void emit2(int,int,int,char*);
void emit3(int,int,int,int);
void emit4(int,int,int,int,int);
void emitFunDef();
void emitFun(int,int,char*,ParamList);
int getIntOpcode(int);
int getCompOpcode(int);
InstrList getInstrList();
DataList getDataList();
tmpTable getTmpTable();

// Incr temp and label
int newTemp();
int newLable();
void popTemp(int);
void initTmpTable();

// Translation Functions
void transProgram(Program);
void transProc(Proc);
void transDecl(Decl);
void transStm(Stm);
void transCond(Exp, int, int);
void transExp(Exp, int);
void transFunCall(char*, ExpList, int);

// Print Functions
void printInstr(Instr);
void printInstructionList(InstrList list);
void printTmp(int);
void printLabel(int);
void printParams(ParamList);

#endif
