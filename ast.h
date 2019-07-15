// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P
#ifndef _AST
#define _AST

#include "common_defs.h"
#include "parser_preprocess.h"
#include "parser.h"

typedef struct _astNode {
	gterm tag;
	gterm dataType; //for type checking expressions
	char *recordID; //if dataType is TK_RECORDID
	token t;
	struct _astNode *parent, *nextSibling, *firstChild;
} astNode;

astNode * newASTNode(gterm tag, astNode *firstChild);
astNode * newASTLeaf(gterm tag, token t);
astNode * generateAST();
// Needs to be called after generateAST() if parent
// pointers are needed
void populateParentPtrs(astNode *root);
int  getAndPrintASTMemoryUsage();
void printAST(astNode *root);
#endif
