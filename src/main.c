#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "intermediateCode.h"
#include "symTable.h"
#include "tmpTable.h"
#include "toMips.h"
#include "parser.tab.h"

extern Program getAST();
extern void printASTAux();
extern void printTableAux();

int main(void) {
	yyparse();
	printf("Abstract syntax tree:\n"); printASTAux();
	printf("\n");
	printf("\nItermediate Code:\n");
	initTmpTable();
	Program ast = getAST();
	transProgram(ast);
	printInstructionList(getInstrList());
	printMIPS(getInstrList(),getDataList());
	return 0;
}
