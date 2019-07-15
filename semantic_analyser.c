// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "semantic_analyser.h"
#include "table_id.h"
#include "table_rec.h"
#include "table_func.h"
#include "type_extraction.h"

#define TABLE_SIZE (1<<9)

idTable gidt;
functionTable ft;
recordTable rt;

extern globalVar* globalVarListHead ;
extern globalVar* globalVarListTail ;


bool hasSemanticError;


void semanticErrorCheckFunction(astNode *f);
void semanticErrorCheckMain(astNode *f);
void semancticErrorCheckStmts(astNode *s, functionTableEntry *fte, bool *isAssigned);
void semanticErrorCheckOtherStmts(astNode *os, functionTableEntry *fte, bool *isAssigned);
void semanticErrorCheckAssignmentStmt(astNode *as, functionTableEntry *fte, bool *isAssigned);
void semanticErrorCheckIterativeStmt(astNode *is, functionTableEntry *fte, bool *isAssigned);
void semanticErrorCheckConditionalStmt(astNode *cs, functionTableEntry *fte, bool *isAssigned);
void semanticErrorCheckIOStmt(astNode *ios, functionTableEntry *fte, bool *isAssigned);
void semanticErrorCheckFunCallStmt(astNode *fcs, functionTableEntry *fte, bool *isAssigned);
void semanticErrorCheckBooleanExpression(astNode *be, functionTableEntry *fte,int line);
void semanticErrorCheckRelationalExpression(astNode *re, functionTableEntry *fte,int line);
void semanticErrorCheckArithmeticExpression(astNode *ae, functionTableEntry *fte, int line);
gterm getFieldType(typeInfo *ti, char* field);
void semanticErrorCheckReturnStmt(astNode *rs, functionTableEntry *fte, bool *isAssigned);
void checkFunctionOpParameter(char *id, typeInfo *outputPars, bool *isAssigned);
bool isValidAllVarUse(astNode *var, functionTableEntry *fte);
bool isValidVarUse(token id, functionTableEntry *fte);
bool isValidRecordVarUse(token id, token field, functionTableEntry *fte);
void printLocalVariables(astNode *decs, functionTableEntry *fte);
void printParameters(typeInfo *p, functionTableEntry *fte);


void initSemanticAnalyser() {
	gidt = createIDT(TABLE_SIZE);
	ft = createFT(TABLE_SIZE);
	rt = createRT(TABLE_SIZE);
	hasSemanticError = false;
}

void semanticAnalyser(astNode *ast) {
	//collect all records defined anywhere in the program
	typeExtractionRec(ast);

	//collect global variables defined anywhere in the program
	typeExtractionGlobalVars(ast);

	//collect function definitions

	astNode *f = ast->firstChild;
	while(f->tag == function) {
		typeExtractionFunc(f);
		typeExtractionLocalVars(f);
		semanticErrorCheckFunction(f);
		f = f->nextSibling;
	}
	semanticErrorCheckMain(f);
}

void semanticErrorCheckFunction(astNode *f) {
	assert(f->tag == function);
	functionTableEntry *fte = searchInFT(ft, f->firstChild->t.lexeme);
	bool *isAssigned = (bool*)malloc(sizeof(bool)*(fte->numOutput));
	semancticErrorCheckStmts(f->firstChild->nextSibling->nextSibling->nextSibling, fte, isAssigned);
	int i;
	typeInfo *iter = fte->outputPars;
	bool flag = true;
	for(i=0; i<fte->numOutput; ++i) {
		if(isAssigned[i]==false)
		{
			//ERROR: output parameter is not assigned
			hasSemanticError = true;
			if(flag) {
				fprintf(stdout, "\nERROR END_OF_FUNCTION '%s'\n", fte->id.lexeme);
				fprintf(stdout, "\tUnitialised parameters\n");
			}
			fprintf(stdout, "\tParameter: '%s'\n",iter->id.lexeme);

		}
		iter = iter->next;
	}
	free(isAssigned);
}

void semanticErrorCheckMain(astNode *f) {
	assert(f->tag == mainFunction);
	functionTableEntry e;
	e.key = (char*)malloc(sizeof(char)*6);
	strcpy(e.key,"_main");
	token t;
	t.tid = TK_MAIN;
	t.lexeme = e.key;
	t.numVal = NULL;
	t.lineNo = -1;
	f->t=t;
	e.id = t;
	e.numInput = 0;
	e.numOutput = 0;
	e.outputPars = NULL;
	e.inputPars = NULL;
	e.frameSize=0;
	e.lidt = createIDT(TABLE_SIZE);
	ft = insertInFT(ft,e);

	//extract local variables of main
	typeExtractionLocalVars(f);

	functionTableEntry *fte = searchInFT(ft, "_main");

	semancticErrorCheckStmts(f->firstChild, fte, NULL);
}

void semancticErrorCheckStmts(astNode *s, functionTableEntry *fte, bool *isAssigned) {
	assert(s->tag == stmts);
	astNode *os = s->firstChild->nextSibling->nextSibling;
	assert(os->tag == otherStmts);
	semanticErrorCheckOtherStmts(os,fte,isAssigned);
	semanticErrorCheckReturnStmt(os->nextSibling, fte,isAssigned);
}

void semanticErrorCheckOtherStmts(astNode *os, functionTableEntry *fte, bool *isAssigned) {
	astNode *iter = os->firstChild;
	while(iter != NULL) {
		if(iter->tag == assignmentStmt)
			semanticErrorCheckAssignmentStmt(iter,fte,isAssigned);
		else if(iter->tag == iterativeStmt)
			semanticErrorCheckIterativeStmt(iter,fte,isAssigned);
		else if(iter->tag == conditionalStmt)
			semanticErrorCheckConditionalStmt(iter,fte,isAssigned);
		else if(iter->tag == TK_READ || iter->tag == TK_WRITE)
			semanticErrorCheckIOStmt(iter,fte,isAssigned);
		else if(iter->tag == funCallStmt)
			semanticErrorCheckFunCallStmt(iter,fte,isAssigned);
		else
			assert(0);
		iter = iter->nextSibling;
	}
}

void semanticErrorCheckAssignmentStmt(astNode *as, functionTableEntry *fte, bool *isAssigned) {
	assert(as->tag == assignmentStmt);
	astNode *lhs = as->firstChild;
	astNode *rhs = lhs->nextSibling;

	assert(lhs->tag == singleOrRecId);

	if(isValidAllVarUse(lhs, fte)) {
		checkFunctionOpParameter(lhs->firstChild->t.lexeme, fte->outputPars, isAssigned);

		idTableEntry *ie = searchInIDT(fte->lidt, lhs->firstChild->t.lexeme);
		idTableEntry *ge = searchInIDT(gidt, lhs->firstChild->t.lexeme);
		ie = ie == NULL ? ge : ie;
		if(lhs->firstChild->nextSibling == NULL) {
			lhs->dataType = ie->dataType;
			if(ie->dataType == TK_RECORDID) {
				lhs->recordID = ie->rptr->id.lexeme;
			}
			else {
				lhs->recordID = NULL;
			}
		}
		else{
			lhs->dataType = getFieldType(ie->rptr->fields, lhs->firstChild->nextSibling->t.lexeme);
			lhs->recordID = NULL;
		}
	}
	else
		//undeclared variable, fuck all behaviour
		lhs->dataType = SENTINEL;

	int line = lhs->firstChild->t.lineNo;
	semanticErrorCheckArithmeticExpression(rhs, fte, line);

	if(lhs->dataType != rhs->dataType) {
		//error
		fprintf(stdout, "\nLine %d: ASSIGNMENT_TYPE_ERROR\n",line );
		fprintf(stdout, "\tLHS data type: '%s' RHS data type: '%s'\n", enumToStr(lhs->dataType), enumToStr(rhs->dataType));
	}
}

void semanticErrorCheckIterativeStmt(astNode *is, functionTableEntry *fte, bool *isAssigned) {
	assert(is->tag == iterativeStmt);

	astNode *condition = is->firstChild;
	astNode *body = condition->nextSibling;
	assert(body->tag == otherStmts);

	semanticErrorCheckBooleanExpression(condition, fte,is->t.lineNo);
	semanticErrorCheckOtherStmts(body, fte, isAssigned);
}

void semanticErrorCheckConditionalStmt(astNode *cs, functionTableEntry *fte, bool *isAssigned) {
	assert(cs->tag == conditionalStmt);
	astNode *condition = cs->firstChild;
	astNode *ifPart = condition->nextSibling;
	assert(ifPart->tag == TK_IF);
	astNode *elsePart = ifPart->nextSibling;
	assert(elsePart->tag == TK_ELSE);

	semanticErrorCheckBooleanExpression(condition, fte, cs->t.lineNo);
	semanticErrorCheckOtherStmts(ifPart, fte, isAssigned);
	semanticErrorCheckOtherStmts(elsePart, fte, isAssigned);
}

void semanticErrorCheckIOStmt(astNode *ios, functionTableEntry *fte, bool *isAssigned) {
	/*ERRORS detected
	1) if record field is being accesed, check if it is a defined field
	2) variable in scope or not
	*/
	astNode *var = ios->firstChild;
	if(ios->tag == TK_READ) {
		assert(var->tag == singleOrRecId);
		if(isValidAllVarUse(var, fte))
			checkFunctionOpParameter(var->firstChild->t.lexeme, fte->outputPars, isAssigned);
	}
	else
		isValidAllVarUse(var, fte);
}

void semanticErrorCheckFunCallStmt(astNode *fcs, functionTableEntry *fte, bool *isAssigned) {
	/*ERRORS detected
	1) recursion
	2) function declared or not
	3) input output parameter matching
	*/
	assert(fcs->tag == funCallStmt);
	astNode *fid = fcs->firstChild;
	if(strcmp(fid->t.lexeme, fte->id.lexeme) == 0) {
		//ERROR: Recursion
		hasSemanticError = true;
		fprintf(stdout, "\nLine %d: RECURSIVE_CALL_ERROR\n",fid->t.lineNo );
		fprintf(stdout, "\tRecursive call to '%s' detected\n", fid->t.lexeme);

		//not processing any more errors on this function call statement
		return;
	}

	functionTableEntry *called = searchInFT(ft,fid->t.lexeme);
	if(called == NULL) {
		//ERROR: undeclared function
		hasSemanticError = true;
		fprintf(stdout, "\nLine %d: UNDEFINED_FUNCTION_ERROR\n",fid->t.lineNo );
		fprintf(stdout, "\tCall made to undefined function '%s'\n", fid->t.lexeme);
		return;
	}

	//only normal variables can occur as parameters , not "record_ID.field"
	astNode *actualOP = fid->nextSibling->firstChild;
	typeInfo *formalOP = called->outputPars;
	int i;
	for(i=0; i<called->numOutput; ++i) {
		if(actualOP == NULL) {
			break;
		}
		if(isValidVarUse(actualOP->t, fte))
			checkFunctionOpParameter(actualOP->t.lexeme, fte->outputPars, isAssigned);
		else {
			actualOP = actualOP->nextSibling;
			formalOP = formalOP->next;
			continue;
		}
		idTableEntry *ie = searchInIDT(fte->lidt, actualOP->t.lexeme);
		idTableEntry *ge = searchInIDT(gidt, actualOP->t.lexeme);
		ie = ie ==NULL ? ge : ie;
		if(ie->dataType != formalOP->dataType) {
			//ERROR: type of formal and actual parameters don't match
			hasSemanticError = true;
			fprintf(stdout, "\nLine %d: PARAMATER_TYPE_ERROR\n", actualOP->t.lineNo);
			fprintf(stdout, "\tFunction '%s' expects an output parameter of type '%s' at position %d\n",called->id.lexeme, enumToStr(formalOP->dataType),i);
			fprintf(stdout, "\tReceived parameter '%s' of type '%s'\n",actualOP->t.lexeme, enumToStr(ie->dataType));
		}
		else if(ie->dataType == TK_RECORDID && formalOP->dataType == TK_RECORDID) {
			if(strcmp(ie->rptr->id.lexeme, formalOP->recordID)!=0) {
				//ERROR: type mismatch
				hasSemanticError = true;
				fprintf(stdout, "\nLine %d: PARAMATER_TYPE_ERROR\n", actualOP->t.lineNo);
				fprintf(stdout, "\tFunction '%s' expects an output parameter of type '%s' at position %d\n",called->id.lexeme, formalOP->recordID,i);
				fprintf(stdout, "\tReceived parameter '%s' of type '%s'\n",actualOP->t.lexeme, ie->rptr->id.lexeme);
			}
		}
		actualOP = actualOP->nextSibling;
		formalOP = formalOP->next;
	}

	if(i < called->numOutput || actualOP != NULL) {
		//ERROR: number of parameters don't match
		hasSemanticError = true;
		fprintf(stdout, "\nLine %d: PARAMATER_NUMBER_ERROR\n", fid->t.lineNo);
		fprintf(stdout, "\tIncorrect number of output parameters supplied to function '%s'\n",called->id.lexeme);
		fprintf(stdout, "\tExpected %d parameters\n",called->numOutput);
	}

	astNode *actualIP = fid->nextSibling->nextSibling->firstChild;
	typeInfo *formalIP = called->inputPars;
	for(i=0; i<called->numInput; ++i) {
		if(actualIP == NULL) {
			break;
		}
		isValidVarUse(actualIP->t, fte);
		idTableEntry *ie = searchInIDT(fte->lidt, actualIP->t.lexeme);
		idTableEntry *ge = searchInIDT(gidt, actualIP->t.lexeme);
		ie = ie ==NULL ? ge : ie;
		if(ie->dataType != formalIP->dataType) {
			//ERROR: type of formal and actual parameters don't match
			hasSemanticError = true;
			fprintf(stdout, "\nLine %d: PARAMATER_TYPE_ERROR\n", actualIP->t.lineNo);
			fprintf(stdout, "\tFunction '%s' expects an input parameter of type '%s' at position %d\n",called->id.lexeme, enumToStr(formalIP->dataType),i);
			fprintf(stdout, "\tReceived parameter '%s' of type '%s'\n",actualIP->t.lexeme, enumToStr(ie->dataType));
		}
		else if(ie->dataType == TK_RECORDID) {
			if(strcmp(ie->rptr->id.lexeme, formalIP->recordID)!=0) {
				//ERROR: type mismatch
				hasSemanticError = true;
				fprintf(stdout, "\nLine %d: PARAMATER_TYPE_ERROR\n", actualIP->t.lineNo);
				fprintf(stdout, "\tFunction '%s' expects an input parameter of type '%s' at position %d\n",called->id.lexeme, formalIP->recordID,i);
				fprintf(stdout, "\tReceived parameter '%s' of type '%s'\n",actualIP->t.lexeme, ie->rptr->id.lexeme);
			}
		}
		actualIP = actualIP->nextSibling;
		formalIP = formalIP->next;
	}

	if(i < called->numInput || actualIP != NULL) {
		//ERROR: number of parameters don't match
		hasSemanticError = true;
		fprintf(stdout, "\nLine %d: PARAMATER_NUMBER_ERROR\n", fid->t.lineNo);
		fprintf(stdout, "\tIncorrect number of input parameters supplied to function '%s'\n",called->id.lexeme);
		fprintf(stdout, "\tExpected %d parameters\n",called->numOutput);
	}
}

void semanticErrorCheckBooleanExpression(astNode *be, functionTableEntry *fte,int line) {
	if(be->tag == TK_AND || be->tag == TK_OR) {
		astNode *left = be->firstChild;
		astNode *right = left->nextSibling;
		semanticErrorCheckBooleanExpression(left, fte, line);
		semanticErrorCheckBooleanExpression(right, fte, line);
	}
	else if(be->tag == TK_NOT) {
		semanticErrorCheckBooleanExpression(be->firstChild, fte, line);
	}
	else if(be->tag == TK_LE || be->tag == TK_LT || be->tag == TK_GE || be->tag == TK_GT || be->tag == TK_EQ || be->tag == TK_NE)
		semanticErrorCheckRelationalExpression(be,fte, line);
	else
		assert(0);

}

void semanticErrorCheckRelationalExpression(astNode *re, functionTableEntry *fte,int line) {
	token left = re->firstChild->t;
	token right = re->firstChild->nextSibling->t;
	if(isValidVarUse(left,fte) && isValidVarUse(right,fte)) {
		if((left.tid == TK_NUM && right.tid == TK_RNUM) || (left.tid == TK_RNUM && right.tid == TK_NUM)) {
			//error, type mismatch
			hasSemanticError = true;
			fprintf(stdout, "\nLine %d: OPERAND_TYPE_MISMATCH_ERROR\n",line );
			fprintf(stdout, "\t'%s' received two operands of types '%s' and '%s'\n",enumToStr(re->tag),enumToStr(left.tid), enumToStr(right.tid) );
		}
		else if(((left.tid == TK_NUM || left.tid == TK_RNUM)&& right.tid == TK_ID) || ((right.tid == TK_NUM || right.tid == TK_RNUM)&& left.tid == TK_ID)) {
			token id = left.tid == TK_ID ? left : right;
			token num = left.tid == TK_ID ? right : left;
			idTableEntry *ie = searchInIDT(fte->lidt, id.lexeme);
			idTableEntry *ge = searchInIDT(gidt, id.lexeme);
			ie = ie == NULL ? ge : ie;
			if(ie->dataType == TK_RECORDID) {
				//ERROR
				hasSemanticError = true;
				fprintf(stdout, "\nLine %d: OPERAND_TYPE_MISMATCH_ERROR\n",line );
				fprintf(stdout, "\tVariable '%s' of record type '%s' appearing as operand to relational operation '%s'\n",id.lexeme,ie->rptr->id.lexeme,enumToStr(re->tag) );
			}
			else {
				if((ie->dataType ==TK_INT && num.tid == TK_RNUM) || (ie->dataType == TK_REAL && num.tid == TK_INT)) {
					//ERROR
					hasSemanticError = true;
					fprintf(stdout, "\nLine %d: OPERAND_TYPE_MISMATCH_ERROR\n",line );
					fprintf(stdout, "\tRelational operator '%s' received operand of type '%s' and '%s'\n",enumToStr(re->tag), enumToStr(ie->dataType), enumToStr(num.tid) );

				}
			}
		}
	}

}

void semanticErrorCheckArithmeticExpression(astNode *ae, functionTableEntry *fte, int line) {
	if(ae->tag == TK_PLUS || ae->tag == TK_MINUS){
		astNode *left = ae->firstChild;
		astNode *right = left->nextSibling;
		semanticErrorCheckArithmeticExpression(left, fte,line);
		semanticErrorCheckArithmeticExpression(right, fte,line);
		if(left->dataType != right->dataType) {
			//ERROR
			hasSemanticError = true;
			fprintf(stdout, "\nLine %d: OPERAND_TYPE_MISMATCH_ERROR\n",line );
			char *leftType, *rightType;
			if(left->dataType != TK_RECORDID)
				leftType = enumToStr(left->dataType);
			else
				leftType = left->recordID;
			if(right->dataType != TK_RECORDID)
				rightType = enumToStr(right->dataType);
			else
				rightType = right->recordID;
			fprintf(stdout, "\t'%s' received two operands of types '%s' and '%s'\n",enumToStr(ae->tag),leftType, rightType );
		}
		else if(left->dataType == TK_RECORDID) {
			if(strcmp(left->recordID, right->recordID) != 0) {
				//error
				hasSemanticError = true;
				fprintf(stdout, "\nLine %d: OPERAND_TYPE_MISMATCH_ERROR\n",line );
				fprintf(stdout, "\t'%s' received two operands of types '%s' and '%s'\n",enumToStr(ae->tag),left->recordID, right->recordID );
			}
			else {
				ae->dataType = left->dataType;
				ae->recordID = left->recordID;
			}
		}
		else {
			ae->dataType = left->dataType;
			ae->recordID = left->recordID;
		}
	}
	else if(ae->tag == TK_MUL || ae->tag == TK_DIV) {
		astNode *left = ae->firstChild;
		astNode *right = left->nextSibling;
		semanticErrorCheckArithmeticExpression(left, fte,line);
		semanticErrorCheckArithmeticExpression(right, fte,line);
		if(left->dataType == right->dataType) {
			if(left->dataType == TK_RECORDID) {
				//ERROR: two records mul/div
				hasSemanticError = true;
				fprintf(stdout, "\nLine %d: OPERAND_TYPE_MISMATCH_ERROR\n",line );
				fprintf(stdout, "\t'%s' received two operands of types '%s' and '%s'\n",enumToStr(ae->tag),left->recordID, right->recordID );
				fprintf(stdout, "\t[Two records even of same record type cannot be multiplied or divided]\n");
			}
			else {
				ae->dataType = left->dataType;
				ae->recordID = left->recordID;
			}
		}
		else if(left->dataType == TK_RECORDID && right->dataType != TK_RECORDID) {
			ae->dataType = left->dataType;
			ae->recordID = left->recordID;
		}
		else if(left->dataType != TK_RECORDID && right->dataType == TK_RECORDID) {
			ae->dataType = right->dataType;
			ae->recordID = right->recordID;
		}
		else {
			//ERROR
			hasSemanticError = true;
			fprintf(stdout, "\nLine %d: OPERAND_TYPE_MISMATCH_ERROR\n",line );
			fprintf(stdout, "\t'%s' received two operands of types '%s' and '%s'\n",enumToStr(ae->tag),enumToStr(left->dataType), enumToStr(right->dataType) );
		}
	}
	else if(ae->tag == all) {
		if(isValidAllVarUse(ae, fte)) {
			idTableEntry *ie = searchInIDT(fte->lidt, ae->firstChild->t.lexeme);
			idTableEntry *ge = searchInIDT(gidt, ae->firstChild->t.lexeme);
			ie = ie == NULL ? ge : ie;
			if(ae->firstChild->nextSibling == NULL) {
				ae->dataType = ie->dataType;
				if(ae->dataType == TK_RECORDID) {
					ae->recordID = ie->rptr->id.lexeme;
				}
				else {
					ae->recordID = NULL;
				}
			}
			else{
				ae->dataType = getFieldType(ie->rptr->fields, ae->firstChild->nextSibling->t.lexeme);
				ae->recordID = NULL;
			}
		}
		else
			//undeclared variable, undefined all behaviour
			ae->dataType = SENTINEL;
	}
	else if(ae->tag == TK_NUM){
		ae->dataType = TK_INT;
		ae->recordID = NULL;
	}
	else if(ae->tag == TK_RNUM) {
		ae->dataType = TK_REAL;
		ae->recordID = NULL;
	}
	else
		assert(0);
}

gterm getFieldType(typeInfo *ti, char* field) {
	while(ti != NULL) {
		if(strcmp(ti->id.lexeme, field)==0)
			return ti->dataType;
		ti = ti->next;
	}
	assert(0);
	return SENTINEL;
}

void semanticErrorCheckReturnStmt(astNode *rs, functionTableEntry *fte, bool *isAssigned) {
	assert(rs->tag == returnStmt);
	typeInfo *iter = fte->outputPars;
	for(int i=0; i<fte->numOutput; ++i) {
		if(isAssigned[i] == false){
			hasSemanticError = true;
			fprintf(stdout, "\nERROR END_OF_FUNCTION '%s'\n", fte->id.lexeme );
			fprintf(stdout, "\tReached end of function '%s', output parameter '%s' is un-initialised within the function\n",fte->id.lexeme, iter->id.lexeme);
		}
		iter = iter->next;
	}

	if(rs->firstChild == NULL && fte->numOutput != 0) {
		//error;
		hasSemanticError = true;
		fprintf(stdout, "\nERROR END_OF_FUNCTION '%s'\n", fte->id.lexeme );
		fprintf(stdout, "\tReached end of non void function '%s', no parameters returned\n",fte->id.lexeme);
	}
	else if(rs->firstChild != NULL && fte->numOutput == 0) {
		//error;
		hasSemanticError = true;
		fprintf(stdout, "\nERROR END_OF_FUNCTION '%s'\n", fte->id.lexeme );
		fprintf(stdout, "\tReached end of void function '%s', invalid parameters returned\n",fte->id.lexeme);
	}
	else if(rs->firstChild != NULL) {
		astNode *iter = rs->firstChild;
		typeInfo *oiter = fte->outputPars;
		while(iter != NULL && oiter != NULL) {
			if(strcmp(iter->t.lexeme, oiter->id.lexeme)!=0) {
				hasSemanticError = true;
				fprintf(stdout, "\nERROR END_OF_FUNCTION '%s'\n", fte->id.lexeme );
				fprintf(stdout, "\tReached end of function '%s', invalid parameter '%s' returned. Expected '%s'\n",fte->id.lexeme, iter->t.lexeme, oiter->id.lexeme);
			}
			iter=iter->nextSibling;
			oiter = oiter->next;
		}
		if((iter == NULL && oiter != NULL) || (iter != NULL && oiter == NULL) ) {
			fprintf(stdout, "\nERROR\n" );
			fprintf(stdout, "\nERROR END_OF_FUNCTION '%s'\n", fte->id.lexeme );
			fprintf(stdout, "\tReached end of function '%s', invalid number of parameters returned. Expected '%d'\n",fte->id.lexeme, fte->numOutput);

		}
	}


}

void checkFunctionOpParameter(char *id, typeInfo *outputPars, bool *isAssigned) {
	//to check if id is a return parameter of function, if yes, set isAssigned
	int i=0;
	while(outputPars != NULL) {
		if(strcmp(id, outputPars->id.lexeme) == 0)
			isAssigned[i] = true;
		outputPars = outputPars->next;
		i++;
	}
}

bool isValidAllVarUse(astNode *var, functionTableEntry *fte) {
	if(var->firstChild == NULL) //TK_NUM or TK_RNUM, no need to check scope
		return true;
	astNode *temp = var->firstChild;
	if(temp->nextSibling == NULL) { //if it temp is NULL normal variable temp would be null
		return isValidVarUse(var->firstChild->t, fte);
	}
	else { //<record var>.<field id>
		return isValidRecordVarUse(var->firstChild->t, var->firstChild->nextSibling->t,fte);
	}
}

bool isValidVarUse(token id, functionTableEntry *fte) {
	if(id.tid == TK_NUM || id.tid == TK_RNUM)
		return true;
	idTableEntry *ie, *ge;
	ie = searchInIDT(fte->lidt, id.lexeme);
	ge = searchInIDT(gidt, id.lexeme);
	if(ie == NULL && ge == NULL) {
		//ERROR: variable out of scope
		hasSemanticError = true;
		fprintf(stdout, "\nLine %d: UNDECLARED_VARIABLE_ERROR\n", id.lineNo);
		fprintf(stdout, "\tVariable '%s' is beyond the scope of the function '%s'\n",id.lexeme, fte->id.lexeme );
		return false;
	}
	return true;
}

bool isValidRecordVarUse(token id, token field, functionTableEntry *fte) {
	idTableEntry *ie, *ge, *e;
	ie = searchInIDT(fte->lidt, id.lexeme);
	ge = searchInIDT(gidt, id.lexeme);
	if(ie == NULL && ge == NULL) {
		//ERROR: variable out of scope
		fprintf(stdout, "\nLine %d: UNDECLARED_VARIABLE_ERROR\n", id.lineNo);
		fprintf(stdout, "\tVariable '%s' is beyond the scope of the function '%s'\n",id.lexeme, fte->id.lexeme );
		return false;
	}
	e = ie == NULL ? ge : ie;

	if(e->dataType != TK_RECORDID) {
		//ERROR: . operator used on int/real
		fprintf(stdout, "\nLine %d: UNDEFINED_MEMBER_ERROR\n", id.lineNo);
		fprintf(stdout, "\tVariable '%s' of type '%s' has no member with the name '%s'\n",id.lexeme,enumToStr(e->dataType),field.lexeme);
		return false;
	}
	else {
		typeInfo *fields = e->rptr->fields;
		while(fields != NULL) {
			if(strcmp(fields->id.lexeme, field.lexeme) == 0)
				break;
			fields = fields->next;
		}
		if(fields == NULL) {
			//ERROR: unknown member accessed
			fprintf(stdout, "\nLine %d: UNDEFINED_MEMBER_ERROR\n", id.lineNo);
			fprintf(stdout, "\tVariable '%s' of type '%s' has no member with the name '%s'\n",id.lexeme,e->rptr->id.lexeme,field.lexeme);
			return false;
		}
	}
	return true;
}

void printAllVariables(astNode *ast) {
	astNode *f = ast->firstChild;
	printf("Lexeme\ttype\tscope\toffset\n");
	//first print global variables
	functionTableEntry *fte;
	while(f->tag == function) {
		fte = searchInFT(ft, f->firstChild->t.lexeme);
		astNode *s = f->firstChild->nextSibling->nextSibling->nextSibling;
		assert(s->tag == stmts);
		astNode *decs = s->firstChild->nextSibling;
		printParameters(fte->inputPars, fte);
		printParameters(fte->outputPars, fte);
		printLocalVariables(decs, fte);
		f = f->nextSibling;
	}
	fte = searchInFT(ft, "_main");
	printLocalVariables(f->firstChild->firstChild->nextSibling, fte);

}

void printLocalVariables(astNode *decs, functionTableEntry *fte) {
	assert(decs->tag == declarations);
	astNode *d = decs->firstChild;
	while(d != NULL) {
		astNode *id = d->firstChild->nextSibling;
		idTableEntry *ie;
		ie = searchInIDT(fte->lidt, id->t.lexeme);
		if(id->nextSibling == NULL) { // local variabl
			if(ie->dataType != TK_RECORDID)
				printf("%s\t%s\t%s\t%d\n",ie->id.lexeme, enumToStr(ie->dataType), fte->id.lexeme, ie->offset);
			else
				printf("%s\t%s\t%s\t%d\n",ie->id.lexeme, ie->rptr->id.lexeme, fte->id.lexeme, ie->offset);
		}
		else { //global variable
			astNode *id = d->firstChild->nextSibling;
			idTableEntry *ie;
			ie = searchInIDT(gidt, id->t.lexeme);
			if(ie->dataType != TK_RECORDID)
				printf("%s\t%s\t%s\t---\n",ie->id.lexeme, enumToStr(ie->dataType),"gloabl");
			else
				printf("%s\t%s\t%s\t---\n",ie->id.lexeme, ie->rptr->id.lexeme,"global");
		}
		d = d->nextSibling;
	}
}

void printParameters(typeInfo *p, functionTableEntry *fte) {
	while(p != NULL) {
		idTableEntry *ie = searchInIDT(fte->lidt, p->id.lexeme);
		if(ie->dataType != TK_RECORDID)
			printf("%s\t%s\t%s\t%d\n",ie->id.lexeme, enumToStr(ie->dataType), fte->id.lexeme, ie->offset);
		else
			printf("%s\t%s\t%s\t%d\n",ie->id.lexeme, ie->rptr->id.lexeme, fte->id.lexeme, ie->offset);
		p = p->next;
	}
}
