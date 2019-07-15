// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#ifndef _PARSER
#define _PARSER

#include "lexer.h"
#include "parser_preprocess.h"
#include <stdbool.h>

struct _astNode;
typedef struct _astNode astNode;

typedef struct _treeNode {
	token t;
	int rhsUsed;
	astNode *node, *inh;
	struct _treeNode *parent, *nextSibling, *firstChild;
} treeNode;

extern bool hasParsingError;

void generateParseTree(grammar g); //this will create a parse tree for the given file with the global var as the root
void printParseTree(FILE * fptr); // Prints the parse tree to the given fileName
treeNode * getParseTree();
int getAndPrintParseTreeMemoryUsage();
#endif
