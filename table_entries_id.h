#ifndef _TABLE_ENTRIES_ID
#define _TABLE_ENTRIES_ID

#include "table_entries_rec.h"

typedef struct _idTableEntry {
	char *key;
	token id;
	gterm dataType;
	int size;
	int offset; //offset of variable within scope of this table
	recordTableEntry *rptr;
} idTableEntry;

#endif
