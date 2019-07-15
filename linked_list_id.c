// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common_defs.h"
#include "linked_list_id.h"

linkedListID createListID() {
	linkedListID l;
	l.head = NULL;
	l.size = 0;
	return l;
}

linkedListID destroyListID(linkedListID l) {
	nodeID *curr = l.head;
	while (curr != NULL) {
		nodeID *temp = curr;
		curr = curr->next;
		free(temp);
	}
	l.head = NULL;
	l.size = 0;
	return l;
}

linkedListID insertInListID(linkedListID l, idTableEntry e) {
	nodeID *newNode = malloc(sizeof(nodeID));
	newNode->e = e;
	newNode->next = l.head;
	l.head = newNode;
	++l.size;
	return l;
}

idTableEntry * searchInListID(linkedListID l, char *key) {
	nodeID *curr = l.head;
	while (curr != NULL) {
		if (strcmp(key, curr->e.key) == 0) {
			return &(curr->e);
		}
		curr = curr->next;
	}
	return NULL;
}

void printListID(linkedListID l) {
	nodeID *curr = l.head;
	int i = 0;
	fprintf(stderr, "Printing linked list\n");
	while (curr != NULL) {
		fprintf(stderr, "Node #%d: %s\n", i, curr->e.key);
		++i;
		curr = curr->next;
	}
	fprintf(stderr, "Done printing linked list\n");
}
