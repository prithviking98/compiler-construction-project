// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "type_extraction.h"

extern recordTable rt;
extern bool hasSemanticError;

typeInfo * isIDUsed(typeInfo *fields, char *id) {
	typeInfo *temp = fields;
	while(temp!=NULL) {
		if(strcmp(temp->id.lexeme,id))
			return temp;
		temp = temp->next;
	}
	return NULL;
}

void getRecord(astNode *ast) {
	assert(ast->tag == typeDefinition);
	//this ast node is of the type "typeDefintion"
	//to first check if this exists in the recordTable
	//if not, then check if the variables in it are defined properly (i.e. not repeated or global in nature)
	//if both conditions are fine, then add to the table
	recordTableEntry *rte = searchInRT(rt,ast->firstChild->t.lexeme);
	if(rte != NULL) {
		fprintf(stdout,"\nLine:%d: RE_DEFINITION_ERROR\n",ast->firstChild->t.lineNo);
		fprintf(stdout,"\tRecord '%s' has already been defined on line%d\n",ast->firstChild->t.lexeme,rte->id.lineNo);
		hasSemanticError = true;
		return;
	}
	recordTableEntry e;
	e.key = ast->firstChild->t.lexeme;
	e.id = ast->firstChild->t;
	e.fields = NULL;
	e.size = 0;
	astNode *temp = ast->firstChild->nextSibling->firstChild;
	typeInfo *first, *iter;
	first = NULL;
	iter = NULL;
	while(temp!=NULL) {
		//temp right now pointing to the type and the next one is fieldid
		typeInfo *add_field = malloc(sizeof(typeInfo));
		if(first == NULL) {
			first = add_field;
			iter = add_field;
		}
		else {
			iter->next = add_field;
			iter = iter->next;
		}
		add_field->dataType = temp->t.tid;
		if(add_field->dataType == TK_INT)
			e.size+=2;
		else
			e.size+=4;
		add_field->id = temp->nextSibling->t;
		add_field->next = NULL;
		add_field->recordID = NULL; //record fields can't be other records
		//check if a field of this name has already been added
		typeInfo *ti=isIDUsed(e.fields,add_field->id.lexeme);
		if(ti != NULL) {
			fprintf(stdout,"\nLine:%d: RE_DECLARATION_ERROR\n",add_field->id.lineNo);
			fprintf(stdout,"\tField '%s' has already been declared within this record on line %d\n",add_field->id.lexeme,ti->id.lineNo);
			hasSemanticError = true;
		}
		temp = temp->nextSibling->nextSibling;
	}
	e.fields = first;
	rt = insertInRT(rt,e);
}

void typeExtractionRec(astNode *ast) {
	if(ast->tag == program) {
		astNode *temp = ast->firstChild;
		while(temp!=NULL) {
			typeExtractionRec(temp);
			temp = temp->nextSibling;
		}
	}
	else if(ast->tag == mainFunction) {
		typeExtractionRec(ast->firstChild);
	}
	else if(ast->tag == function) {
		if(ast->firstChild->nextSibling->nextSibling->tag == stmts) //ouput_par node may not be present
			typeExtractionRec(ast->firstChild->nextSibling->nextSibling);
		else
			typeExtractionRec(ast->firstChild->nextSibling->nextSibling->nextSibling);
	}
	else if(ast->tag == stmts) {
		typeExtractionRec(ast->firstChild);
	}
	else if(ast->tag == typeDefinitions) {
		astNode *temp = ast->firstChild;
		while(temp!=NULL) {
			typeExtractionRec(temp);
			temp = temp->nextSibling;
		}
	}
	else if(ast->tag == typeDefinition) {
		//this is where a record entry is actually being declared
		getRecord(ast);
	}
	else
		assert(0);
}

void printRecordEntry(recordTableEntry entry){
	printf("\n%s\t\t%d\t",entry.id.lexeme,entry.size);
	typeInfo* field = entry.fields;
	while(field!=NULL){

		if(field->dataType==TK_INT)
			printf("int");
		if(field->dataType==TK_REAL)
			printf("real");
		field=field->next;
		if(field != NULL)
			printf(", ");
	}
}

void printExtractedRecs(){
	int currBinNum=0,currRecNum=0;
	linkedListRec currBin;
	nodeRec* currRecEntry;
	printf("\nRecordID---------Size----------Fields---------------\n");
	for(;currBinNum<rt.size;currBinNum++){
		currBin = rt.bins[currBinNum];
		currRecEntry = currBin.head;
		for(currRecNum=0;currRecNum<currBin.size;currRecNum++){
			printRecordEntry(currRecEntry->e);
			currRecEntry = currRecEntry->next;
		}
	}
	printf("\n------------------------------------------------------\n");
}
