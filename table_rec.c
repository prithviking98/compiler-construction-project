// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#include "table_rec.h"
#include <stdio.h>
#include <stdlib.h>
#include "hash_function.h"

recordTable createRT(int size) {
	recordTable rt;
	rt.size=size;
	rt.total=0;
	rt.bins=(linkedListRec*)malloc(sizeof(linkedListRec)*size);
	for(int i=0; i<size; ++i) {
		rt.bins[i]=createListRec();
	}
	return rt;
}

recordTable destroyRT(recordTable rt) {
	for(int i=0; i<rt.size; ++i) {
		rt.bins[i]=destroyListRec(rt.bins[i]); }
	free(rt.bins);
	rt.bins=NULL;
	rt.size=0;
	rt.total=0;
	return rt;
}

recordTableEntry * searchInRT(recordTable rt, char *key) {
	int i = hash(key, rt.size);
	return searchInListRec(rt.bins[i], key);
}

recordTable insertInRT(recordTable rt, recordTableEntry e) {
	if(searchInRT(rt, e.key)!= NULL) //do nothing if already present
		return rt;
	int bin= hash(e.key,rt.size);
	rt.total++;
	rt.bins[bin]=insertInListRec(rt.bins[bin], e);
	return rt;
}

void printRT(recordTable rt) {
	fprintf(stderr, "Printing Hash Table\n");
	for(int i=0; i<rt.size; ++i) {
		if(rt.bins[i].size==0)
			continue;
		fprintf(stderr, "Bin %d\n",i);
		printListRec(rt.bins[i]);
	}
}
