// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#include "table_id.h"
#include <stdio.h>
#include <stdlib.h>
#include "hash_function.h"

idTable createIDT(int size) {
	idTable idt;
	idt.size=size;
	idt.total=0;
	idt.bins=(linkedListID*)malloc(sizeof(linkedListID)*size);
	for(int i=0; i<size; ++i) {
		idt.bins[i]=createListID();
	}
	return idt;
}

idTable destroyIDT(idTable idt) {
	for(int i=0; i<idt.size; ++i) {
		idt.bins[i]=destroyListID(idt.bins[i]); }
	free(idt.bins);
	idt.bins=NULL;
	idt.size=0;
	idt.total=0;
	return idt;
}

idTableEntry * searchInIDT(idTable idt, char *key) {
	int i = hash(key, idt.size);
	return searchInListID(idt.bins[i], key);
}

idTable insertInIDT(idTable idt, idTableEntry e) {
	if(searchInIDT(idt, e.key)!= NULL) //do nothing if already present
		return idt;
	idt.total++;
	int bin= hash(e.key,idt.size);
	idt.bins[bin]=insertInListID(idt.bins[bin], e);
	return idt;
}

void printIDT(idTable idt) {
	fprintf(stderr, "Printing Hash Table\n");
	for(int i=0; i<idt.size; ++i) {
		if(idt.bins[i].size==0)
			continue;
		fprintf(stderr, "Bin %d\n",i);
		printListID(idt.bins[i]);
	}
}
