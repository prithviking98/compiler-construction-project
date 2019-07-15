// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common_defs.h"
#include "linked_list_func.h"

linkedListFunc createListFunc() {
	linkedListFunc l;
	l.head = NULL;
	l.size = 0;
	return l;
}

linkedListFunc destroyListFunc(linkedListFunc l) {
	nodeFunc *curr = l.head;
	while (curr != NULL) {
		nodeFunc *temp = curr;
		curr = curr->next;
		free(temp);
	}
	l.head = NULL;
	l.size = 0;
	return l;
}

linkedListFunc insertInListFunc(linkedListFunc l, functionTableEntry e) {
	nodeFunc *newNode = malloc(sizeof(nodeFunc));
	newNode->e = e;
	newNode->next = l.head;
	l.head = newNode;
	++l.size;
	return l;
}

functionTableEntry * searchInListFunc(linkedListFunc l, char *key) {
	nodeFunc *curr = l.head;
	while (curr != NULL) {
		if (strcmp(key, curr->e.key) == 0) {
			return &curr->e;
		}
		curr = curr->next;
	}
	return NULL;
}

void printListFunc(linkedListFunc l) {
	nodeFunc *curr = l.head;
	int i = 0;
	fprintf(stderr, "Printing linked list\n");
	while (curr != NULL) {
		fprintf(stderr, "Node #%d: %s\n", i, curr->e.key);
		++i;
		curr = curr->next;
	}
	fprintf(stderr, "Done printing linked list\n");
}
