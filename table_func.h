// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P

#ifndef _TABLE_FUNC
#define _TABLE_FUNC

#include "common_defs.h"
#include "linked_list_func.h"
#include "lexer.h"

typedef struct _functionTable {
	linkedListFunc *bins;
	int size; //number of bins
	int total; //total number of elements
} functionTable;

//hash function in hash_table.c

functionTable createFT(int size);

functionTable destroyFT(functionTable ft);

functionTableEntry * searchInFT(functionTable ft, char *key);

functionTable insertInFT(functionTable ft, functionTableEntry e);

void printFT(functionTable ft);
//prints all non empty bins (for testing)

#endif
