#ifndef _TABLE_ENTRIES_REC
#define _TABLE_ENTRIES_REC

#include "common_defs.h"
#include "lexer.h"

typedef struct _typeInfo {
	gterm dataType;
	char* recordID; //in case dataType is TK_RECORDID
	token id;
	int size;
	struct _typeInfo *next;
} typeInfo;

typedef struct _recordTableEntry {
	char *key; //record ID
	token id;
	typeInfo *fields; //list of struct fields
	int size;
} recordTableEntry;

#endif
