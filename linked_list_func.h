// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P

#ifndef _LINKED_LIST_FUNC
#define _LINKED_LIST_FUNC

#include "common_defs.h"
#include "lexer.h"
#include "table_entries_func.h"

typedef struct _nodeFunc {
	functionTableEntry e;
	struct _nodeFunc *next;
} nodeFunc;

typedef struct _linkedListFunc {
	nodeFunc *head;
	int size;
} linkedListFunc;

linkedListFunc createListFunc();

linkedListFunc destroyListFunc(linkedListFunc l); //delete all elements, free stuff

linkedListFunc insertInListFunc(linkedListFunc l, functionTableEntry e); //do malloc inside for node, makes usage clean

functionTableEntry * searchInListFunc(linkedListFunc l, char *value);

void printListFunc(linkedListFunc l); //for testing

#endif
