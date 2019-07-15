// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P

#ifndef _LINKED_LIST_ID
#define _LINKED_LIST_ID

#include "common_defs.h"
#include "lexer.h"
#include "table_entries_id.h"

typedef struct _nodeID {
	idTableEntry e;
	struct _nodeID *next;
} nodeID;

typedef struct _linkedListID {
	nodeID *head;
	int size;
} linkedListID;

linkedListID createListID();

linkedListID destroyListID(linkedListID l); //delete all elements, free stuff

linkedListID insertInListID(linkedListID l, idTableEntry e); //do malloc inside for node, makes usage clean

idTableEntry * searchInListID(linkedListID l, char *value);

void printListID(linkedListID l); //for testing

#endif
