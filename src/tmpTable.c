#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tmpTable.h"
#include "parser.tab.h"

tmpTable tmpPush(tmpTable table, char* id, int tmp){
	tmpTable ret = (tmpTable)malloc(sizeof(struct _stack));
	char *s = malloc(strlen(id)+1);
	strcpy(s, id);
	ret->ident = s;
	ret->tmpValue = tmp;
	ret->bottom = table;
	return ret;
}

tmpTable tmpLookup(tmpTable table, char* id){
	if(NULL == table){ return NULL; }
	tmpTable cur = (tmpTable)malloc(sizeof(struct _stack));
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

void printTmpTable(tmpTable table){
	tmpTable cur = (tmpTable)malloc(sizeof(struct _stack));
	cur = table;
	while(NULL != cur){
		printf("\nIdentifier: %s\nTempId: %d\n", cur->ident, cur->tmpValue);
		cur = cur->bottom;
	}
}
