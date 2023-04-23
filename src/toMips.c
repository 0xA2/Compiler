#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toMips.h"
#include "parser.tab.h"

extern void tmerror();

void printMIPS(InstrList list, DataList data){

	printf("\nMIPS Assembly:\n");

	// Print .data
	if(data){
		printf("\n.data\n");
		DataList curData = (DataList)malloc(sizeof(struct _datalist));
		curData = data;
		while(NULL != curData){
			printf("\t%s: .asciiz %s\n", curData->id, curData->string);
			curData = curData->next;
		}
	}


	// Print .text

	printf("\n.text\n");
	if(!list){ tmerror("trying to generate assembly from empty list"); }

	InstrList cur = (InstrList)malloc(sizeof(struct _instrlist));
	cur = list;
	while(NULL != cur){
		switch(cur->in->opcode){
			case OPCODE_DEF:
				printMIPSFunDef(cur->in);
				break;
			case OPCODE_CALL:
				printMIPSFunCall(cur->in);
				break;
			case OPCODE_RETURN:
				printMIPSFunRet(cur->in);
				break;
			case OPCODE_LABEL:
				printMIPSLabel(cur->in);
				break;
			case OPCODE_JUMP:
				printMIPSJump(cur->in);
				break;
			case OPCODE_MOVE:
				printMIPSMove(cur->in);
				break;
			case OPCODE_MOVEI:
				printMIPSMoveI(cur->in);
				break;
			case OPCODE_MOVES:
				break;
			case OPCODE_MOVEB:
				printMIPSMoveI(cur->in);
				break;
			case OPCODE_ADD:
				printMIPSOp(cur->in);
				break;
			case OPCODE_SUB:
				printMIPSOp(cur->in);
				break;
			case OPCODE_MULT:
				printMIPSOp(cur->in);
				break;
			case OPCODE_DIV:
				printMIPSOp(cur->in);
				break;
			case OPCODE_MOD:
				printMIPSOp(cur->in);
				break;
			case OPCODE_EQ:
				printMIPSCond(cur->in);
				break;
			case OPCODE_NOTEQ:
				printMIPSCond(cur->in);
				break;
			case OPCODE_LT:
				printMIPSCond(cur->in);
				break;
			case OPCODE_LEQT:
				printMIPSCond(cur->in);
				break;
			case OPCODE_GT:
				printMIPSCond(cur->in);
				break;
			case OPCODE_GEQT:
				printMIPSCond(cur->in);
				break;
			default:
				tmerror("unrecognized instruction");
		}
		cur = cur->next;
	}
}


void printReadInt(){
	printf("\nreadint:\n");
	printf("\tli $v0, 5\n");
	printf("\tsyscall\n");
	printf("\tjr $ra\n");
}

void printWriteInt(){
	printf("\nwriteint:\n");
	printf("\tli $v0, 1\n");
	printf("\tlw $a0, 0($sp)");
	printf("\tsyscall\n");
	printf("\tjr $ra\n");
}

void printWriteString(){
	printf("\nwritestring:\n");
	printf("\tli $v0, 4\n");
	printf("\tlw $a0, 0($sp)");
	printf("\tsyscall\n");
	printf("\tjr $ra\n");
}

void printMIPSLabel(Instr in){
	if(in->arg1 == 0){ printf("main:\n"); return; }
	printf("\n"); printLabel(in->arg1); printf(":\n");
}

void printMIPSJump(Instr in){
	printf("\tj "); printLabel(in->arg1); printf("\n");
}

void printMIPSFunDef(Instr in){
	printf("%s:\n", in->ident);
	printf("\tsw $fp, -4($sp)\n");
	printf("\tsw $ra, -8($sp)\n");
	printf("\tmove $fp, $sp\n");
	printf("\taddiu $sp, $sp,-9\n");
}

void printMIPSFunRet(Instr in){
	printf("\tmove $v0, $"); printTmp(in->arg1); printf("\n");
	printf("return_"); printTmp(in->arg1); printf("\n");
	printf("\tmove $sp, $fp\n");
	printf("\tlw $ra, -8($sp)\n");
	printf("\tlw $fp, -4($sp)\n");
	printf("\tjr $ra\n\n");
}

void printMIPSFunCall(Instr in){
	tmpTable tmp = (tmpTable)malloc(sizeof(struct _stack));
	ParamList cur = (ParamList)malloc(sizeof(struct _paramlist));
	cur = in->list;
	while(NULL != cur){ tmp = tmpPush(tmp,"",cur->value);cur = cur->next; }
	int k = 4;
	tmpTable curParam = (tmpTable)malloc(sizeof(struct _stack));
	curParam = tmp;
	while(NULL != curParam){
		printf("\tsw "); printTmp(curParam->tmpValue); printf(", -%d($sp)\n", k);
		k += 4;
		curParam = curParam->bottom;
	}
	printf("\tjal %s\n", in->ident);
	printf("\taddiu $sp, $sp, -%d\n", k);
	printf("\tmove t, $v0\n");
	if(strcmp(in->ident,"readint") == 0){ printReadInt(); }
	if(strcmp(in->ident,"writeint") == 0){ printWriteInt(); }
	if(strcmp(in->ident,"writestring") == 0){ printWriteString(); }

}

void printMIPSMove(Instr in){
	printf("\tmove "); printf("$"); printTmp(in->arg1); printf(", "); printf("$"); printTmp(in->arg2); printf("\n");
}

void printMIPSMoveI(Instr in){
	printf("\tli "); printf("$"); printTmp(in->arg1); printf(", "); printf("%d",in->arg2); printf("\n");
}

void printMIPSOp(Instr in){
	switch(in->opcode){
		case OPCODE_ADD:
			printf("\tadd ");  printf("$"); printTmp(in->arg1); printf(", ");  printf("$"); printTmp(in->arg2); printf(", ");  printf("$"); printTmp(in->arg3); printf("\n");
			break;
		case OPCODE_SUB:
			printf("\tsub ");  printf("$"); printTmp(in->arg1); printf(", ");  printf("$"); printTmp(in->arg2); printf(", ");  printf("$"); printTmp(in->arg3); printf("\n");
			break;
		case OPCODE_MULT:
			printf("\tmult "); printf("$"); printTmp(in->arg2); printf(", ");  printf("$"); printTmp(in->arg3); printf("\n");
			printf("\tmflo "); printf("$"); printTmp(in->arg1); printf("\n");
			break;
		case OPCODE_DIV:
			printf("\tdiv "); printf("$"); printTmp(in->arg2); printf(", ");  printf("$"); printTmp(in->arg3); printf("\n");
			printf("\tmflo "); printf("$"); printTmp(in->arg1); printf("\n");
			break;
		case OPCODE_MOD:
			printf("\tmod "); printf("$"); printTmp(in->arg2); printf(", ");  printf("$"); printTmp(in->arg3); printf("\n");
			printf("\tmfhi "); printf("$"); printTmp(in->arg1); printf("\n");
			break;
		default:
			tmerror("unrecognized operation");
	}
}

void printMIPSCond(Instr in){
	switch(in->opcode){
		case OPCODE_EQ:
			printf("\tbeq "); printf("$"); printTmp(in->arg1); printf(", "); printf("$"); printTmp(in->arg2); printf(", "); printLabel(in->arg3); printf("\n");
			printf("\tj "); printLabel(in->arg4); printf("\n");
			break;
		case OPCODE_NOTEQ:
			printf("\tbne "); printf("$"); printTmp(in->arg1); printf(", "); printf("$"); printTmp(in->arg2); printf(", "); printLabel(in->arg3); printf("\n");
			printf("\tj "); printLabel(in->arg4); printf("\n");
			break;
		case OPCODE_LT:
			printf("\tblt "); printf("$"); printTmp(in->arg1); printf(", "); printf("$"); printTmp(in->arg2); printf(", "); printLabel(in->arg3); printf("\n");
			printf("\tj "); printLabel(in->arg4); printf("\n");
			break;
		case OPCODE_LEQT:
			printf("\tble "); printf("$"); printTmp(in->arg1); printf(", "); printf("$"); printTmp(in->arg2); printf(", "); printLabel(in->arg3); printf("\n");
			printf("\tj "); printLabel(in->arg4); printf("\n");
			break;
		case OPCODE_GT:
			printf("\tbgt "); printf("$"); printTmp(in->arg1); printf(", "); printf("$"); printTmp(in->arg2); printf(", "); printLabel(in->arg3); printf("\n");
			printf("\tj "); printLabel(in->arg4); printf("\n");
			break;
		case OPCODE_GEQT:
			printf("\tbge "); printf("$"); printTmp(in->arg1); printf(", "); printf("$"); printTmp(in->arg2); printf(", "); printLabel(in->arg3); printf("\n");
			printf("\tj "); printLabel(in->arg4); printf("\n");
			break;
		default:
			tmerror("unrecognized conditional operation");
	}
}
