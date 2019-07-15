// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "type_extraction.h"
#include "table_func.h"
#include "table_rec.h"
#include "table_id.h"
#include "common_defs.h"
#include <assert.h>

#define LIDT_SIZE (1<<9)

extern recordTable rt;
extern functionTable ft;
extern idTable gidt; //global variables
extern bool hasSemanticError;
extern linkedListFunc funList;

//utility function declarations
typeInfo * processParameterList(astNode *pars); //processes AST node of tag input_pars or output_pars
idTable insertInLIDT(idTable lidt, typeInfo *pars, int *frameSize); //inserts collected i/o pars of function into local ID table of func

void typeExtractionFunc(astNode *f) {
	assert(f->tag == function);
	astNode *fname = f->firstChild;
	//checing if function with same name already defined
	functionTableEntry *ftep = searchInFT(ft,fname->t.lexeme);
	if(ftep != NULL) {
		//ERROR: redeclaration
		fprintf(stdout,"\nLine %d: RE_DECLARATION_ERROR\n",fname->t.lineNo);
		fprintf(stdout, "\tFunction '%s\'' first declared on line %d\n",fname->t.lexeme, ftep->id.lineNo);
		hasSemanticError = true;
	}
	else {
		functionTableEntry e;
		e.id = fname->t;
		e.key = fname->t.lexeme;
		e.lidt = createIDT(LIDT_SIZE);
		e.frameSize = 0;
		astNode* inputPars = fname->nextSibling;

		//outpars node may not be present so have to check
		astNode *outputPars = inputPars->nextSibling ;
		e.inputPars = processParameterList(inputPars);
		e.lidt = insertInLIDT(e.lidt, e.inputPars,&(e.frameSize));
		e.numInput = e.lidt.total;
		e.outputPars = processParameterList(outputPars);
		e.lidt = insertInLIDT(e.lidt, e.outputPars,&(e.frameSize));
		e.numOutput = e.lidt.total - e.numInput;
		ft = insertInFT(ft,e);

	}
}

typeInfo * processParameterList(astNode *pars){
	assert(pars->tag==input_par || pars->tag==output_par);
	if(pars->firstChild==NULL)
		return NULL;
	astNode *type;
	type = pars->firstChild;
	typeInfo *first, *iter;
	first = (typeInfo*)malloc(sizeof(typeInfo));
	first->dataType = type->t.tid;
	first->id = type->nextSibling->t;
	if(first->dataType == TK_INT){
		first->recordID = NULL;
		first->size = 2;
	}
	else if(first->dataType == TK_REAL) {
		first->recordID = NULL;
		first->size = 4;
	}
	else if(first->dataType == TK_RECORDID){
		first->recordID = type->t.lexeme;
		first->size = searchInRT(rt,first->recordID)->size;
	}
	else {
		assert(0);
	}
	iter = first;
	iter->next = NULL;
	type = type->nextSibling->nextSibling;
	while(type != NULL) {
		iter->next = (typeInfo*)malloc(sizeof(typeInfo));
		iter = iter->next;
		iter->next = NULL;
		iter->dataType = type->t.tid;
		iter->id = type->nextSibling->t;
		if(iter->dataType == TK_INT) {
			iter->recordID = NULL;
			iter->size = 2;
		}
		else if(iter->dataType == TK_REAL) {
			iter->recordID = NULL;
			iter->size = 4;
		}
		else if(iter->dataType == TK_RECORDID) {
			iter->recordID = type->t.lexeme;
			iter->size = searchInRT(rt,iter->recordID)->size;
		}
		else {
			assert(0);
		}
		type = type->nextSibling->nextSibling;
	}
	return first;
}

idTable insertInLIDT(idTable lidt, typeInfo *pars, int *frameSize) {
	if(pars == NULL)
		return lidt;
	idTableEntry e;
	while(pars != NULL) {
		//checking if parameter name clashes with any global variable
		idTableEntry *ide = searchInIDT(gidt, pars->id.lexeme);
		if(ide != NULL) {
			//ERROR: parameter name clashes with global variable
			fprintf(stdout, "\nLine %d: RE_DECLARATION_ERROR\n", pars->id.lineNo );
			fprintf(stdout, "\tIdentifier '%s' first declared on line %d as a global variable \n",pars->id.lexeme, ide->id.lineNo);
			hasSemanticError = true;
		}
		e.key = pars->id.lexeme;
		e.id = pars->id;
		e.dataType = pars->dataType;
		e.size = pars->size;
		e.offset = *frameSize;
		*frameSize = (*frameSize)+e.size;
		if(e.dataType == TK_RECORDID)
			e.rptr = searchInRT(rt, pars->recordID);
		lidt = insertInIDT(lidt, e);
		pars = pars->next;
	}
	return lidt;
}

void printFunc(functionTableEntry* e){
	printf("%s\t\t%d\n",e->key,e->frameSize);
}

void printFuncs(astNode * root){
	assert(root->tag==program);
	astNode * curr = root->firstChild;
	printf("=| Identifier |======| Frame size |======\n");
	while (curr->tag!= mainFunction) {
		printFunc(searchInFT(ft,curr->firstChild->t.lexeme));
		curr = curr->nextSibling;
	}
	printFunc(searchInFT(ft,"_main"));
}
