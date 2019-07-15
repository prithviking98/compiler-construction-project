// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common_defs.h"
#include "linked_list_rec.h"

linkedListRec createListRec() {
	linkedListRec l;
	l.head = NULL;
	l.size = 0;
	return l;
}

linkedListRec destroyListRec(linkedListRec l) {
	nodeRec *curr = l.head;
	while (curr != NULL) {
		nodeRec *temp = curr;
		curr = curr->next;
		free(temp);
	}
	l.head = NULL;
	l.size = 0;
	return l;
}

linkedListRec insertInListRec(linkedListRec l, recordTableEntry e) {
	nodeRec *newNode = malloc(sizeof(nodeRec));
	newNode->e = e;
	newNode->next = l.head;
	l.head = newNode;
	++l.size;
	return l;
}

recordTableEntry * searchInListRec(linkedListRec l, char *key) {
	nodeRec *curr = l.head;
	while (curr != NULL) {
		if (strcmp(key, curr->e.key) == 0) {
			return &curr->e;
		}
		curr = curr->next;
	}
	return NULL;
}

void printListRec(linkedListRec l) {
	nodeRec *curr = l.head;
	int i = 0;
	fprintf(stderr, "Printing linked list\n");
	while (curr != NULL) {
		fprintf(stderr, "Node #%d: %s\n", i, curr->e.key);
		++i;
		curr = curr->next;
	}
	fprintf(stderr, "Done printing linked list\n");
}
