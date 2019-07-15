// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P
#include <stdio.h>
#include <stdlib.h>
#include "type_extraction.h"
#include <stdbool.h>
#include "table_id.h"
#include "table_rec.h"
#include "table_func.h"
#include "assert.h"

extern idTable gidt;
extern functionTable ft;
extern recordTable rt;
extern bool hasSemanticError;

globalVar* globalVarListHead = NULL;
globalVar* globalVarListTail = NULL;

void extractLocal(astNode *stmts, functionTableEntry *fte);
void extractGlobal(astNode *stmts,int *offset);

void updateGlobalVarList(char *id,int offset,gterm term){
	globalVar *temp=malloc(sizeof(globalVar));
	temp->id=id;
	temp->offset=offset;
	if(term == TK_INT){
		temp->type = "Int";
	}
	else{
		temp->type = "Real";
	}
	temp->next=NULL;
	if (globalVarListHead==NULL){
		globalVarListHead=temp;
		globalVarListTail=temp;
	}
	else{
		globalVarListTail->next=temp;
		globalVarListTail=temp;
	}
}

void printGlobalVarList(){
	globalVar* curr=globalVarListHead;
	if(curr ==NULL){
		printf("No Global Variables\n");
		return;
	}
	printf("\n| Identifier |=====| Type |=====| Offset |\n");
	while(curr!=NULL){
		printf("%s\t\t%s\t\t%d\n",curr->id,curr->type,curr->offset);
		curr=curr->next;
	}
}

void typeExtractionLocalVars(astNode *f) {
	assert(f->tag == function || f->tag == mainFunction);
	if(f->tag == mainFunction) {
		functionTableEntry *fte = searchInFT(ft, "_main");
		extractLocal(f->firstChild, fte);
	}
	else {
		functionTableEntry *fte = searchInFT(ft, f->firstChild->t.lexeme);
		extractLocal(f->firstChild->nextSibling->nextSibling->nextSibling,fte);
	}

}

void extractLocal(astNode *s, functionTableEntry *fte) {
	assert(s->tag == stmts);
	astNode *tds = s->firstChild->nextSibling;
	astNode *iter = tds->firstChild;
	idTableEntry e, *ie, *gie;
	while(iter != NULL) {
		if(iter->firstChild->nextSibling->nextSibling == NULL) {
			astNode* temp = iter->firstChild;
			ie = searchInIDT(fte->lidt, temp->nextSibling->t.lexeme);
			gie = searchInIDT(gidt, temp->nextSibling->t.lexeme);
			if(gie != NULL) {
				//ERROR: redeclaration
				hasSemanticError = true;
				fprintf(stdout, "\nLine %d: RE_DECLARATION_ERROR\n",temp->nextSibling->t.lineNo );
				fprintf(stdout, "\tIdentifier '%s' already declared on line %d\n",gie->id.lexeme, gie->id.lineNo );
				iter = iter->nextSibling;
				continue;
			}
			else if(ie != NULL) {
				//ERROR: redeclaration
				hasSemanticError = true;
				fprintf(stdout, "\nLine %d: RE_DECLARATION_ERROR\n",temp->nextSibling->t.lineNo );
				fprintf(stdout, "\tIdentifier '%s' already declared on line %d\n",ie->id.lexeme, ie->id.lineNo );
				iter = iter->nextSibling;
				continue;
			}
			e.id = temp->nextSibling->t;
			e.key = e.id.lexeme;
			e.dataType = temp->t.tid;
			e.rptr = NULL;
			e.offset = fte->frameSize;
			if(e.dataType == TK_INT)
				e.size=2;
			else if(e.dataType == TK_REAL)
				e.size=4;
			else {
				e.rptr = searchInRT(rt,temp->t.lexeme);
				e.size = e.rptr->size;
			}
			fte->frameSize += e.size;
			fte->lidt = insertInIDT(fte->lidt,e);
		}
		iter = iter->nextSibling;
	}

}

void typeExtractionGlobalVars(astNode *ast) {
	assert(ast->tag == program);
	astNode *iter = ast->firstChild;
	int offset=0;
	while(iter->tag != mainFunction) {
		if(iter->tag == function){
			extractGlobal(iter->firstChild->nextSibling->nextSibling->nextSibling,&offset);
		}
		iter = iter->nextSibling;
	}
	assert(iter->tag == mainFunction);
	extractGlobal(iter->firstChild, &offset);
}

void extractGlobal(astNode *s,int *offset) {
	assert(s->tag == stmts);
	astNode *tds = s->firstChild->nextSibling;
	astNode *iter = tds->firstChild;
	idTableEntry e,*ie;
	while(iter != NULL) {
		if(iter->firstChild->nextSibling->nextSibling != NULL) { //means it is a global variable
			astNode* temp = iter->firstChild;
			ie = searchInIDT(gidt, temp->nextSibling->t.lexeme);
			if(ie != NULL) {
				//ERROR: redeclaration
				hasSemanticError = true;
				fprintf(stdout, "\nLine %d: RE_DECLARATION_ERROR\n",temp->nextSibling->t.lineNo );
				fprintf(stdout, "\tIdentifier '%s' already declared on line %d\n",ie->id.lexeme, ie->id.lineNo );
				iter = iter->nextSibling;
				continue;
			}
			e.id = temp->nextSibling->t;
			e.key = e.id.lexeme;
			e.dataType = temp->t.tid;
			e.rptr = NULL;
			e.offset = *offset;
			if(e.dataType == TK_INT)
				e.size=2;
			else if(e.dataType == TK_REAL)
				e.size=4;
			else {
				e.rptr = searchInRT(rt,temp->t.lexeme);
				e.size = e.rptr->size;
			}
			*offset += e.size;
			gidt = insertInIDT(gidt,e);
			//insert into globalVar list for inorder printing
			updateGlobalVarList(e.key,e.offset,e.dataType);
		}
		iter = iter->nextSibling;
	}
}
