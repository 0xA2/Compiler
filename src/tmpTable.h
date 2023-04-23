#ifndef _TMP_TABLE_H_
#define _TMP_TABLE_H_

#include "symTable.h"

// -- Lookup Stack Struct -- //
typedef struct _stack{
	char* ident;
	int tmpValue;
	struct _stack *bottom;
}*tmpTable;

// Stack Functions
tmpTable tmpPush(tmpTable,char*,int);
tmpTable tmpLookup(tmpTable,char*);
void printTmpTable(tmpTable);
#endif
