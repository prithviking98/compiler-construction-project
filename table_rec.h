// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P

#ifndef _TABLE_REC
#define _TABLE_REC

#include "common_defs.h"
#include "linked_list_rec.h"
#include "lexer.h"

typedef struct _recordTable {
	linkedListRec *bins;
	int size; //number of bins
	int total; //total number of elements
} recordTable;

//hash function in hash_table.c

recordTable createRT(int size);

recordTable destroyRT(recordTable rt);

recordTableEntry * searchInRT(recordTable rt, char *key);

recordTable insertInRT(recordTable rt, recordTableEntry e);

void printRT(recordTable rt);
//prints all non empty bins (for testing)

#endif
