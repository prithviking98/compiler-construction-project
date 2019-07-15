// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#ifndef _LINKED_LIST_REC
#define _LINKED_LIST_REC

#include "common_defs.h"
#include "lexer.h"
#include "table_entries_rec.h"

typedef struct _nodeRec {
	recordTableEntry e;
	struct _nodeRec *next;
} nodeRec;

typedef struct _linkedListRec {
	nodeRec *head;
	int size;
} linkedListRec;

linkedListRec createListRec();

linkedListRec destroyListRec(linkedListRec l); //delete all elements, free stuff

linkedListRec insertInListRec(linkedListRec l, recordTableEntry e); //do malloc inside for node, makes usage clean

recordTableEntry * searchInListRec(linkedListRec l, char *value);

void printListRec(linkedListRec l); //for testing

#endif
