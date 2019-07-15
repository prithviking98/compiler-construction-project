// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P
#include <stdlib.h>
#include "common_defs.h"
#include "ast.h"
#include "parser.h"
#include <assert.h>

int numASTNodes;
int ASTmemUsage;

astNode * newASTNode(gterm tag, astNode *firstChild) {
	astNode *node = malloc(sizeof(astNode));
	node->tag = tag;
	node->parent = node->nextSibling = NULL;
	node->firstChild = firstChild;
	numASTNodes++;
	ASTmemUsage+=sizeof(astNode);
	return node;
}
//token already has gterm as an attribute, passing it again is redundant
astNode * newASTLeaf(gterm tag, token t) {
	astNode *leaf = malloc(sizeof(astNode));
	leaf->tag = tag;
	leaf->t = t;
	leaf->parent = leaf->nextSibling = leaf->firstChild = NULL;
	numASTNodes++;
	ASTmemUsage+=sizeof(astNode);
	return leaf;
}
int  getAndPrintASTMemoryUsage(){
	printf("\nAST:\nNumber of nodes = %d\nAllocated Memory = %d Bytes\n",numASTNodes,ASTmemUsage);
	return ASTmemUsage;
}
void generateASTUtil(treeNode *root) {
	// What all needs to be set to NULL?
	// Maybe set everything to NULL in the functions that create an AST node

	//<program> ===> <otherFunctions> <mainFunction>
	if (root->t.tid == program && root->rhsUsed == 0) {
		treeNode *child1 = root->firstChild;
		treeNode *child2 = child1->nextSibling;
		generateASTUtil(child1);
		generateASTUtil(child2);
		if(child1->node == NULL) {
			//there are NO other functions
			root->node = newASTNode(program, child2->node);
		}
		else {
			//there are some other functions
			astNode *temp = child1->node;
			while(temp->nextSibling != NULL)
				temp = temp->nextSibling;
			temp->nextSibling = child2->node;
			root->node = newASTNode(program, child1->node);
		}
		/*child1->node->nextSibling = child2->node;
		root->node = newASTNode(program, child1->node);*/
	}
	//<mainFunction>===> TK_MAIN <stmts> TK_END
	else if (root->t.tid == mainFunction && root->rhsUsed == 0) {
		treeNode *child2 = root->firstChild->nextSibling;
		generateASTUtil(child2);
		root->node = newASTNode(mainFunction, child2->node);
	}
	//<otherFunctions>===> <function><otherFunctions>
	else if (root->t.tid == otherFunctions && root->rhsUsed == 0) {
		treeNode *child1 = root->firstChild;
		treeNode *child2 = child1->nextSibling;
		generateASTUtil(child1);
		generateASTUtil(child2);
		child1->node->nextSibling = child2->node;
		root->node = child1->node;
	}
	//<otherFunctions>===> eps
	else if (root->t.tid == otherFunctions && root->rhsUsed == 1) {
		root->node = NULL;
	}
	//<function>===>TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END
	else if (root->t.tid == function && root->rhsUsed == 0){
		treeNode *inpPar = root->firstChild->nextSibling;
		treeNode *opPar  = inpPar->nextSibling;
		treeNode *stmts  = opPar->nextSibling->nextSibling;
		generateASTUtil(inpPar);
		generateASTUtil(opPar);
		generateASTUtil(stmts);
		root->node = newASTNode(function,newASTLeaf(TK_FUNID,root->firstChild->t));
		root->node->firstChild->nextSibling = inpPar->node;
		/*inpPar->node->nextSibling = opPar->node == NULL ? stmts->node:opPar->node;*/
		inpPar->node->nextSibling = opPar->node;
		opPar->node->nextSibling = stmts->node;
		/*if(opPar!=NULL)
			opPar->node->nextSibling = stmts->node;*/
	}
	//<input_par>===>TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
	else if (root->t.tid == input_par && root->rhsUsed == 0){
		treeNode *paramList = root->firstChild->nextSibling->nextSibling->nextSibling->nextSibling;
		generateASTUtil(paramList);
		root->node = newASTNode(input_par,paramList->node);
	}
	//<output_par>===>TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
	else if (root->t.tid == output_par && root->rhsUsed == 0){
		treeNode *paramList = root->firstChild->nextSibling->nextSibling->nextSibling->nextSibling;
		generateASTUtil(paramList);
		root->node = newASTNode(output_par,paramList->node);
	}
	//<output_par>===>eps
	else if (root->t.tid == output_par && root->rhsUsed == 1){
		root->node = newASTNode(output_par,NULL);
		/*root->node = NULL*/
	}
	//<parameter_list>===><dataType> TK_ID <remaining_list>
	else if (root->t.tid == parameter_list && root->rhsUsed == 0){
		treeNode *dtype = root->firstChild;
		treeNode *id = dtype->nextSibling;
		/*treeNode *remList = dtype->nextSibling->nextSibling;*/
		treeNode *remList = id->nextSibling;
		generateASTUtil(remList);
		generateASTUtil(dtype);
		dtype->node->nextSibling = newASTLeaf(TK_ID,id->t);
		dtype->node->nextSibling->nextSibling = remList->node;
		root->node = dtype->node;
	}
	//<dataType>===> <primitiveDatatype>
	else if (root->t.tid == dataType && root->rhsUsed == 0){
		generateASTUtil(root->firstChild);
		root->node = root->firstChild->node;
	}
	//<dataType>===> <constructedDatatype>
	else if (root->t.tid == dataType && root->rhsUsed == 1){
		generateASTUtil(root->firstChild);
		root->node = root->firstChild->node;
	}
	//<primitiveDatatype>===> TK_INT
	else if (root->t.tid == primitiveDatatype && root->rhsUsed == 0){
		root->node = newASTLeaf(TK_INT,root->firstChild->t);
	}
	//<primitiveDatatype>===> TK_REAL
	else if (root->t.tid == primitiveDatatype && root->rhsUsed == 1){
		root->node = newASTLeaf(TK_REAL,root->firstChild->t);
	}
	//<constructedDatatype>===>TK_RECORD TK_RECORDID
	else if (root->t.tid == constructedDatatype && root->rhsUsed == 0){
		root->node = newASTLeaf(TK_RECORDID,root->firstChild->nextSibling->t);
	}
	//<remaining_list>===>TK_COMMA <parameter_list>
	else if (root->t.tid == remaining_list && root->rhsUsed == 0){
		generateASTUtil(root->firstChild->nextSibling);
		root->node = root->firstChild->nextSibling->node;
	}
	//<remaining_list>===>eps
	else if (root->t.tid == remaining_list && root->rhsUsed == 1){
		root->node = NULL;
	}
	//<stmts>===><typeDefinitions> <declarations> <otherStmts><returnStmt>
	else if (root->t.tid == stmts && root->rhsUsed == 0){
		treeNode *tDefs = root->firstChild;
		treeNode *decls = tDefs->nextSibling;
		treeNode *otherSt = decls->nextSibling;
		treeNode *ret = otherSt->nextSibling;
		generateASTUtil(tDefs);
		generateASTUtil(decls);
		generateASTUtil(otherSt);
		generateASTUtil(ret);
		astNode *os = newASTNode(otherStmts,otherSt->node);
		os->nextSibling = ret->node;
		astNode *dec = newASTNode(declarations,decls->node);
		dec->nextSibling= os;
		astNode *td = newASTNode(typeDefinitions,tDefs->node);
		td->nextSibling = dec;
		root->node = newASTNode(stmts,td);
	}
	//typeDefinitions>===><typeDefinition><typeDefinitions1>
	else if (root->t.tid == typeDefinitions && root->rhsUsed == 0){
		treeNode *tdef = root->firstChild;
		treeNode *tdefs1 = tdef->nextSibling;
		generateASTUtil(tdef);
		generateASTUtil(tdefs1);
		root->node = tdef->node;
		tdef->node->nextSibling = tdefs1->node;
	}
	//<typeDefinitions>===>eps
	else if (root->t.tid == typeDefinitions && root->rhsUsed == 1){
		root->node = NULL;
	}
	//<typeDefinition>===>TK_RECORD TK_RECORDID <fieldDefinitions> TK_ENDRECORD TK_SEM
	else if (root->t.tid == typeDefinition && root->rhsUsed == 0){
		treeNode *rID = root->firstChild->nextSibling;
		treeNode *fieldDefs = rID->nextSibling;
		generateASTUtil(fieldDefs);
		astNode *rIDNode = newASTLeaf(TK_RECORDID,rID->t);
		rIDNode->nextSibling = fieldDefs->node;
		root->node = newASTNode(typeDefinition,rIDNode);
	}
	//<fieldDefinitions>===> <fieldDefinition1> <fieldDefinition2> <moreFields>
	else if (root->t.tid == fieldDefinitions && root->rhsUsed == 0){
		treeNode *fieldDef1 = root->firstChild;
		treeNode *fieldDef2 = fieldDef1->nextSibling;
		treeNode *moreF     = fieldDef2->nextSibling;
		generateASTUtil(fieldDef1);
		generateASTUtil(fieldDef2);
		generateASTUtil(moreF);
		root->node = newASTNode(fieldDefinitions,fieldDef1->node);
		fieldDef1->node->nextSibling->nextSibling = fieldDef2->node;
		fieldDef2->node->nextSibling->nextSibling = moreF->node;
	}
	// <fieldDefinition>===> TK_TYPE <primitiveDatatype> TK_COLON TK_FIELDID TK_SEM
	else if (root->t.tid == fieldDefinition && root->rhsUsed == 0){
		treeNode *pDataType = root->firstChild->nextSibling;
		treeNode *fID = pDataType->nextSibling->nextSibling;
		generateASTUtil(pDataType);
		pDataType->node->nextSibling = newASTLeaf(TK_FIELDID,fID->t);
		root->node = pDataType->node;
	}
	// <moreFields>===><fieldDefinition><moreFields1>
	else if (root->t.tid == moreFields && root->rhsUsed == 0){
		treeNode *fDef = root->firstChild;
		treeNode *moreF = fDef->nextSibling;
		generateASTUtil(fDef);
		generateASTUtil(moreF);
		root->node = fDef->node;
		fDef->node->nextSibling->nextSibling = moreF->node;
	}
	// <moreFields>===>eps
	else if (root->t.tid == moreFields && root->rhsUsed == 1){
		root->node = NULL;
	}
	// <declarations> ===> <declaration><declarations1>
	else if (root->t.tid == declarations && root->rhsUsed == 0){
		treeNode *decl = root->firstChild;
		treeNode *decls= decl->nextSibling;
		generateASTUtil(decl);
		generateASTUtil(decls);
		root->node = decl->node;
		decl->node->nextSibling = decls->node;
	}
	//<declarations> ===> eps
	else if (root->t.tid == declarations && root->rhsUsed == 1){
		root->node = NULL;
	}
	//<declaration>===> TK_TYPE <dataType> TK_COLON TK_ID <global_or_not> TK_SEM
	else if (root->t.tid == declaration && root->rhsUsed == 0){
		treeNode *dType = root->firstChild->nextSibling;
		treeNode *tID = dType->nextSibling->nextSibling;
		treeNode *globalNot = tID->nextSibling;
		generateASTUtil(dType);
		generateASTUtil(globalNot);
		root->node = newASTNode(declaration,dType->node);
		dType->node->nextSibling = newASTLeaf(TK_ID,tID->t);
		dType->node->nextSibling->nextSibling = globalNot->node;
	}
	//<global_or_not>===>TK_COLON TK_GLOBAL
	else if (root->t.tid == global_or_not && root->rhsUsed == 0){
		root->node = newASTLeaf(TK_GLOBAL,root->firstChild->nextSibling->t);
	}
	//<global_or_not>===> eps
	else if (root->t.tid == global_or_not && root->rhsUsed == 1){
		root->node = NULL;
	}
	//<otherStmts>===> <stmt><otherStmts1>
	else if (root->t.tid == otherStmts && root->rhsUsed == 0){
		treeNode *st = root->firstChild;
		treeNode *stmts1 = root->firstChild->nextSibling;
		generateASTUtil(st);
		generateASTUtil(stmts1);
		root->node = st->node;
		st->node->nextSibling = stmts1->node;
	}
	//<otherStmts>===> eps
	else if (root->t.tid == otherStmts && root->rhsUsed == 1){
		root->node = NULL;
	}
	//<stmt>===><assignmentStmt>|<iterativeStmt>|<conditionalStmt>|<ioStmt>|<funCallStmt>
	else if (root->t.tid == stmt){
		generateASTUtil(root->firstChild);
		root->node = root->firstChild->node;
	}
	//<assignmentStmt>===><singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
	else if (root->t.tid == assignmentStmt && root->rhsUsed == 0){
		treeNode *singleOrRec = root->firstChild;
		treeNode *arthExp = singleOrRec->nextSibling->nextSibling;
		generateASTUtil(singleOrRec);
		generateASTUtil(arthExp);
		singleOrRec->node->nextSibling = arthExp->node;
		root->node = newASTNode(assignmentStmt,singleOrRec->node);
	}
	//<singleOrRecId>===>TK_ID <new_24>
	else if (root->t.tid == singleOrRecId && root->rhsUsed == 0){
		treeNode *id = root->firstChild;
		treeNode *new24 = id->nextSibling;
		generateASTUtil(new24);
		id->node = newASTLeaf(TK_ID,id->t);
		id->node->nextSibling = new24->node;
		root->node = newASTNode(singleOrRecId,id->node);
	}
	//<new_24> ===> eps
	else if (root->t.tid == new_24 && root->rhsUsed == 0){
		root->node = NULL;
	}
	//<new_24> ===> TK_DOT TK_FIELDID
	else if (root->t.tid == new_24 && root->rhsUsed == 1){
		root->node = newASTLeaf(TK_FIELDID,root->firstChild->nextSibling->t);
	}
	//<funCallStmt>===><outputParameters> TK_CALL TK_FUNID TK_WITH TK_PARAMETERS <inputParameters> TK_SEM
	else if (root->t.tid == funCallStmt && root->rhsUsed == 0){
		treeNode *opPar = root->firstChild;
		treeNode *funID = opPar->nextSibling->nextSibling;
		treeNode *inPar = funID->nextSibling->nextSibling->nextSibling;
		generateASTUtil(opPar);
		generateASTUtil(inPar);
		funID->node = newASTLeaf(TK_FUNID,funID->t);
		root->node = newASTNode(funCallStmt,funID->node);
		funID->node->nextSibling = opPar->node;
		opPar->node->nextSibling = inPar->node;
	}
	//<outputParameters> ==> TK_SQL <idList> TK_SQR TK_ASSIGNOP
	else if (root->t.tid == outputParameters && root->rhsUsed == 0){
		generateASTUtil(root->firstChild->nextSibling);
		root->node = newASTNode(outputParameters,root->firstChild->nextSibling->node);
	}
	//<outputParameters> ==> eps
	else if (root->t.tid == outputParameters && root->rhsUsed == 1){
		root->node = newASTNode(outputParameters,NULL);
	}
	//<inputParameters>===> TK_SQL <idList> TK_SQR
	else if (root->t.tid == inputParameters && root->rhsUsed == 0){
		generateASTUtil(root->firstChild->nextSibling);
		root->node = newASTNode(inputParameters,root->firstChild->nextSibling->node);
	}
	//<iterativeStmt>===> TK_WHILE TK_OP <booleanExpression> TK_CL <stmt><otherStmts> TK_ENDWHILE
	else if (root->t.tid == iterativeStmt && root->rhsUsed == 0){
		treeNode *booleanExp = root->firstChild->nextSibling->nextSibling;
		treeNode *stmt = booleanExp->nextSibling->nextSibling;
		treeNode *oStmts = stmt->nextSibling;
		generateASTUtil(booleanExp);
		generateASTUtil(stmt);
		generateASTUtil(oStmts);
		root->node = newASTNode(iterativeStmt,booleanExp->node);
		//setting line number here
		root->node->t = root->firstChild->t; //TK_WHILE TOKEN
		/*booleanExp->node->nextSibling = stmt->node;
		stmt->node->nextSibling = oStmts->node;*/
		stmt->node->nextSibling = oStmts->node;
		booleanExp->node->nextSibling = newASTNode(otherStmts,stmt->node);
	}
	//<conditionalStmt>===> TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt><otherStmts> <elsePart>
	else if (root->t.tid == conditionalStmt && root->rhsUsed == 0){
		treeNode *boolExp = root->firstChild->nextSibling->nextSibling;
		treeNode *stmt = boolExp->nextSibling->nextSibling->nextSibling;
		treeNode *oStmts = stmt->nextSibling;
		treeNode *elseP = oStmts->nextSibling;
		generateASTUtil(boolExp);
		generateASTUtil(stmt);
		generateASTUtil(oStmts);
		generateASTUtil(elseP);
		/*root->node = newASTNode(TK_IF,boolExp->node);*/
		root->node = newASTNode(conditionalStmt,boolExp->node);
		//setting line number here cause that info will be lost
		root->node->t = root->firstChild->t; //the TK_IF token
		/*boolExp->node->nextSibling = stmt->node;
		stmt->node->nextSibling = oStmts->node;
		oStmts->node->nextSibling = elseP->node;*/
		stmt->node->nextSibling = oStmts->node;
		boolExp->node->nextSibling = newASTNode(TK_IF,stmt->node);
		boolExp->node->nextSibling->nextSibling = elseP->node;
	}
	//<elsePart>===>TK_ELSE <stmt><otherStmts> TK_ENDIF
	else if (root->t.tid == elsePart && root->rhsUsed == 0){
		/*generateASTUtil(root->firstChild);
		generateASTUtil(root->firstChild->nextSibling);
		root->firstChild->node->nextSibling= root->firstChild->nextSibling->node;
		root->node = newASTNode(elsePart,root->firstChild->node);*/
		treeNode *stmt = root->firstChild->nextSibling;
		treeNode *oStmts = stmt->nextSibling;
		generateASTUtil(stmt);
		generateASTUtil(oStmts);
		stmt->node->nextSibling = oStmts->node;
		root->node = newASTNode(TK_ELSE,stmt->node);
	}
	//<elsePart>===>TK_ENDIF
	else if (root->t.tid == elsePart && root->rhsUsed == 1){
		/*root->node = NULL;*/
		root->node = newASTNode(elsePart,NULL);
	}
	// <ioStmt>===> TK_READ TK_OP <singleOrRecId> TK_CL TK_SEM
	else if (root->t.tid == ioStmt && root->rhsUsed == 0){
		treeNode *singleOrRec = root->firstChild->nextSibling->nextSibling;
		generateASTUtil(singleOrRec);
		root->node = newASTNode(TK_READ,singleOrRec->node);
	}
	//<ioStmt>===> TK_WRITE TK_OP <allVar> TK_CL TK_SEM
	else if (root->t.tid == ioStmt && root->rhsUsed == 1){
		treeNode *allVar = root->firstChild->nextSibling->nextSibling;
		generateASTUtil(allVar);
		root->node = newASTNode(TK_WRITE,allVar->node);
	}
	//<allVar>===>TK_NUM | <allVar>===>TK_RNUM
	else if (root->t.tid == allVar && (root->rhsUsed == 0 ||root->rhsUsed == 1)){
		root->node = newASTLeaf(root->firstChild->t.tid,root->firstChild->t);
	}
	//<allVar>===>TK_ID<allVarTemp>
	else if (root->t.tid == allVar && root->rhsUsed == 2){
		treeNode *allVarT = root->firstChild->nextSibling;
		treeNode *id = root->firstChild;
		generateASTUtil(allVarT);
		id->node = newASTLeaf(TK_ID,root->firstChild->t);
		id->node->nextSibling = allVarT->node;
		root->node = newASTNode(allVar,id->node);
	}
	//<allVarTemp>===>TK_DOT TK_FIELDID
	else if (root->t.tid == allVarTemp && root->rhsUsed == 0){
		root->node = newASTLeaf(TK_FIELDID , root->firstChild->nextSibling->t);
	}
	//<allVarTemp>===>eps
	else if (root->t.tid == allVarTemp && root->rhsUsed == 1){
		root->node = NULL;
	}
	//<arithmeticExpression> ===> <term> <expPrime>
	else if (root->t.tid == arithmeticExpression && root->rhsUsed == 0){
		generateASTUtil(root->firstChild);
		root->firstChild->nextSibling->inh = root->firstChild->node;
		generateASTUtil(root->firstChild->nextSibling);
		root->node = root->firstChild->nextSibling->node;
	}
	//<expPrime> ===> <lowPrecedenceOperators> <term> <expPrime1>
	else if (root->t.tid == expPrime && root->rhsUsed == 0){
		treeNode *lowPrecOp = root->firstChild;
		treeNode *term      = lowPrecOp->nextSibling;
		treeNode *expPrime1 = term->nextSibling;
		generateASTUtil(lowPrecOp);
		generateASTUtil(term);
		lowPrecOp->node->firstChild = root->inh;
		expPrime1->inh = term->node;
		generateASTUtil(expPrime1);
		lowPrecOp->node->firstChild->nextSibling = expPrime1->node;
		root->node = lowPrecOp->node;
	}
	//<expPrime> ===> eps
	else if (root->t.tid == expPrime && root->rhsUsed == 1){
		root->node = root->inh;
	}
	// <lowPrecedenceOperators>===> TK_PLUS
	else if (root->t.tid == lowPrecedenceOperators && root->rhsUsed == 0) {
		root->node = newASTNode(TK_PLUS, NULL);
	}
	// <lowPrecedenceOperators>===> TK_MINUS
	else if (root->t.tid == lowPrecedenceOperators && root->rhsUsed == 1) {
		root->node = newASTNode(TK_MINUS, NULL);
	}
	// <term>===> <factor> <termPrime>
	else if (root->t.tid == term && root->rhsUsed == 0) {
		treeNode *child1 = root->firstChild;
		treeNode *child2 = child1->nextSibling;
		generateASTUtil(child1);
		child2->inh = child1->node;
		generateASTUtil(child2);
		root->node = child2->node;
	}
	// <termPrime> ===> <highPrecedenceOperators><factor><termPrime1>
	else if (root->t.tid == termPrime && root->rhsUsed == 0) {
		treeNode *child1 = root->firstChild;
		treeNode *child2 = child1->nextSibling;
		treeNode *child3 = child2->nextSibling;
		generateASTUtil(child1);
		child1->node->firstChild = root->inh;
		generateASTUtil(child2);
		child3->inh = child2->node;
		generateASTUtil(child3);
		child1->node->firstChild->nextSibling = child3->node;
		root->node = child1->node;
	}
	// <termPrime> ===> eps
	else if (root->t.tid == termPrime && root->rhsUsed == 1) {
		// TODO check doc
		root->node = root->inh;
	}
	// <highPrecedenceOperators>===> TK_DIV
	else if (root->t.tid == highPrecedenceOperators && root->rhsUsed == 1) {
		root->node = newASTLeaf(TK_DIV,root->firstChild->t);
	}
	// <highPrecedenceOperators>===> TK_MUL
	else if (root->t.tid == highPrecedenceOperators && root->rhsUsed == 0 ) {
		root->node = newASTLeaf(TK_MUL,root->firstChild->t);
	}
	// <factor> ===> <all>
	else if (root->t.tid == factor && root->rhsUsed == 1) {
		generateASTUtil(root->firstChild);
		root->node = root->firstChild->node;
	}
	// <factor> ===> TK_OP <arithmeticExpression> TK_CL
	else if (root->t.tid == factor && root->rhsUsed == 0) {
		generateASTUtil(root->firstChild->nextSibling);
		root->node = root->firstChild->nextSibling->node;
	}
	// <all>===> TK_ID<temp>
	else if (root->t.tid == all && root->rhsUsed == 0) {
		generateASTUtil(root->firstChild->nextSibling);
		root->firstChild->node = newASTLeaf(TK_ID,root->firstChild->t);
		root->firstChild->node->nextSibling = root->firstChild->nextSibling->node;
		root->node = newASTNode(all,root->firstChild->node);
	}
	// <all>===>TK_NUM
	else if (root->t.tid == all && root->rhsUsed == 1) {
		root->node = newASTLeaf(TK_NUM,root->firstChild->t);
	}
	// <all>===> TK_RNUM
	else if (root->t.tid == all && root->rhsUsed == 2) {
		root->node = newASTLeaf(TK_RNUM,root->firstChild->t);
	}
	// <temp>===> eps
	else if (root->t.tid == temp && root->rhsUsed == 0) {
		root->node = NULL;
	}
	// <temp>===> TK_DOT TK_FIELDID
	else if (root->t.tid == temp && root->rhsUsed == 1) {
		root->node = newASTLeaf(TK_FIELDID, root->firstChild->nextSibling->t);
	}
	// <booleanExpression>===>TK_OP <booleanExpression1> TK_CL <logicalOp> TK_OP <booleanExpression2> TK_CL
	else if (root->t.tid == booleanExpression && root->rhsUsed == 0) {
		treeNode *boolExp1 = root->firstChild->nextSibling ;
		treeNode *logOp = boolExp1->nextSibling->nextSibling;
		treeNode *boolExp2 = logOp->nextSibling->nextSibling;
		generateASTUtil(boolExp1);
		logOp->inh = boolExp1->node;
		generateASTUtil(logOp);
		generateASTUtil(boolExp2);
		boolExp1->node->nextSibling = boolExp2->node;
		root->node = logOp->node;
	}
	// <booleanExpression>===><var1> <relationalOp> <var2>
	else if (root->t.tid == booleanExpression && root->rhsUsed == 1) {
		treeNode *var1 = root->firstChild;
		treeNode *var2 = root->firstChild->nextSibling->nextSibling;
		generateASTUtil(var1);
		generateASTUtil(var2);
		var1->node->nextSibling = var2->node;
		var1->nextSibling->inh = var1->node;
		generateASTUtil(var1->nextSibling);
		root->node = var1->nextSibling->node;
	}
	// <booleanExpression>===>TK_NOT TK_OP <booleanExpression1>TK_CL
	else if (root->t.tid == booleanExpression && root->rhsUsed == 2) {
		generateASTUtil(root->firstChild->nextSibling->nextSibling);
		root->node = newASTNode(TK_NOT,root->firstChild->nextSibling->nextSibling->node);
	}
	// <var>===> TK_ID
	else if (root->t.tid == var && root->rhsUsed == 0) {
		root->node = newASTLeaf(TK_ID, root->firstChild->t);
	}
	// <var>===> TK_NUM
	else if (root->t.tid == var && root->rhsUsed == 1) {
		root->node = newASTLeaf(TK_NUM, root->firstChild->t);
	}
	// <var>===> TK_RNUM
	else if (root->t.tid == var && root->rhsUsed == 2) {
		root->node = newASTLeaf(TK_RNUM,root->firstChild->t);
	}
	// <logicalOp>===>TK_AND
	else if (root->t.tid == logicalOp && root->rhsUsed == 0) {
		root->node = newASTNode(TK_AND,root->inh);
	}
	// <logicalOp>===>TK_OR
	else if (root->t.tid == logicalOp && root->rhsUsed == 1) {
		root->node = newASTNode(TK_OR,root->inh);
	}
	// <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
	else if (root->t.tid == relationalOp){
		root->node = newASTNode(root->firstChild->t.tid,root->inh);
	}
	// <returnStmt>===>TK_RETURN <optionalReturn> TK_SEM
	else if (root->t.tid == returnStmt && root->rhsUsed ==0){
		generateASTUtil(root->firstChild->nextSibling);
		root->node = newASTNode(returnStmt,root->firstChild->nextSibling->node);
	}
	// <optionalReturn>===>TK_SQL <idList> TK_SQR
	else if (root->t.tid == optionalReturn && root->rhsUsed == 0){
		generateASTUtil(root->firstChild->nextSibling);
		root->node = root->firstChild->nextSibling->node;
	}
	// <optionalReturn>===>eps
	else if (root->t.tid == optionalReturn && root->rhsUsed == 1){
		root->node = NULL;
	}
	// <idList>===> TK_ID <more_ids>
	else if (root->t.tid == idList && root->rhsUsed == 0){
		generateASTUtil(root->firstChild->nextSibling);
		root->node = newASTLeaf (TK_ID,root->firstChild->t);
		root->node->nextSibling = root->firstChild->nextSibling->node;
	}
	// <more_ids>===> TK_COMMA <idList>
	else if (root->t.tid == more_ids && root->rhsUsed == 0){
		generateASTUtil(root->firstChild->nextSibling);
		root->node = root->firstChild->nextSibling->node;
	}
	//  <more_ids>===> eps
	else if (root->t.tid == more_ids && root->rhsUsed == 1){
		root->node = NULL;
	}
	else {
		assert(0);
	}
}

astNode * generateAST() {
	numASTNodes = 0;
	ASTmemUsage = 0;
	treeNode *ptroot = getParseTree();
	generateASTUtil(ptroot);
	populateParentPtrs(ptroot->node);
	return ptroot->node;
}

void populateParentPtrs(astNode *root) {
	if (root == NULL) return;
	astNode *curr = root->firstChild;
	while (curr != NULL) {
		curr->parent = root;
		populateParentPtrs(curr);
		curr = curr->nextSibling;
	}
}

void printASTNode(astNode *node){
	if (node->tag == program){
		printf("Program\n");
	}
	else{
		if(node->firstChild!=NULL){
			printf("%s,\t\t%s,\t\t%s\n",enumToStr(node->tag),enumToStr(node->parent->tag),"Not a Leaf");
		}
		else{
			printf("%s,\t\t%s,\t\t%s\n",enumToStr(node->tag),enumToStr(node->parent->tag),node->t.lexeme);
		}
	}
	return;
}

void printAST(astNode* root){
	if (root==NULL){
		return;
	}
	printASTNode(root);
	astNode* curr = root->firstChild;
	while(curr!=NULL){
		printAST(curr);
		curr = curr->nextSibling;
	}
	return ;
}
