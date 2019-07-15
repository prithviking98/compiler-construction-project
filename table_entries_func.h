#ifndef _TABLE_ENTRIES_FUNC
#define _TABLE_ENTRIES_FUNC

#include "table_entries_rec.h"
#include "table_entries_id.h"
#include "table_id.h"

typedef struct _functionTableEntry {
	char *key;
	token id;
	typeInfo *inputPars, *outputPars;
	int frameSize; //total sizes of all local variables+parameters
	int numInput, numOutput; // number of input and output pars
	idTable lidt; //local ID table
} functionTableEntry;

#endif
