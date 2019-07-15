// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P

#ifndef _TABLE_ID
#define _TABLE_ID

#include "common_defs.h"
#include "linked_list_id.h"
#include "lexer.h"

typedef struct _idTable {
	linkedListID *bins;
	int size; //number of bins
	int total; //total number of elements
} idTable;

//hash function in hash_table.c

idTable createIDT(int size);

idTable destroyIDT(idTable idt);

idTableEntry * searchInIDT(idTable idt, char *key);

idTable insertInIDT(idTable idt, idTableEntry e);

void printIDT(idTable idt);
//prints all non empty bins (for testing)

#endif
