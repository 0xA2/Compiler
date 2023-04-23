#ifndef _TO_MIPS_
#define _TO_MIPS_

#include "intermediateCode.h"
#include "tmpTable.h"

void printMIPS(InstrList, DataList);
void printMIPSLabel(Instr);
void printMIPSJump(Instr);
void printMIPSFunDef(Instr);
void printMIPSFunRet(Instr);
void printMIPSFunCall(Instr);
void printMIPSMove(Instr);
void printMIPSMoveI(Instr);
void printMIPSOp(Instr);
void printMIPSCond(Instr);
void printReadInt();
void printWriteInt();
void printWriteString();

#endif
