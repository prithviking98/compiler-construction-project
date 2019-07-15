// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "common_defs.h"
#include "lexer.h"
#include "parser.h"

#define STACK_INIT_SIZE 10

bool hasParsingError = false;
// Needs to be initialized
treeNode *root = NULL;
treeNode **stack = NULL;

int ptRows = eps;
int ptCols = SENTINEL - eps;
extern int parseTable[eps][SENTINEL - eps];

int stackTop;
int currMaxSize;

int numParseTreeNodes;
int parseTreeMemUsage;

treeNode * createNode(token t) {
	treeNode *node = malloc(sizeof(treeNode));
	node->parent = node->nextSibling = node->firstChild = NULL;
	node->t = t;
	node->rhsUsed = -1;
	numParseTreeNodes++;
	parseTreeMemUsage+=sizeof(treeNode);
	return node;
}

void addChild(treeNode *parent, treeNode *child) {
	child->nextSibling = parent->firstChild;
	child->parent = parent;
	parent->firstChild = child;
}

void printParseTreeUtil(treeNode *node, FILE *fptr) {
	if (node == NULL) {
		return;
	}
	printParseTreeUtil(node->firstChild, fptr);
	char *temp1 = (node->t.lexeme == NULL) ? "N/A" : node->t.lexeme;
	char *temp2 = (node->parent == NULL) ? "N/A" : enumToStr(node->parent->t.tid);
	char *temp3 = (node->firstChild == NULL) ? "YES" : "NO";
	char temp4[100];
	if (node->t.tid == TK_NUM) {
		sprintf(temp4, "%d", *(int *)node->t.numVal);
	}
	else if (node->t.tid == TK_RNUM) {
		sprintf(temp4, "%lf", *(double *)node->t.numVal);
	}
	else {
		sprintf(temp4, "N/A");
	}
	fprintf(fptr, "%s\t%s\t%d\t%s\t%s\t%s\n", temp1, enumToStr(node->t.tid), node->t.lineNo, temp4, temp2, temp3);
	printParseTreeUtil(node->nextSibling, fptr);
}

void printParseTree(FILE *fptr) {
	//we are assuming fptr is stdout for stage 2
	assert(fptr != NULL);
	fprintf(fptr, "Lexeme\tToken\tLine Number\tValue\tParent\tIs Leaf\n");
	printParseTreeUtil(root, fptr);
}

void pushToken(token t) {
	//this will add the token to the stack, return the new ptr and update current size
	treeNode *add = createNode(t);
	if(stackTop == currMaxSize-1) {
		currMaxSize *= 2;
		stack = realloc(stack, sizeof(treeNode*)*currMaxSize);
	}
	stackTop++;
	stack[stackTop] = add;
}

void popToken() {
	//simply decrements the current size
	if(stackTop < 0)
		return;
	stackTop--;
}

void initializeStack() {
	stack = malloc(sizeof(treeNode*) * STACK_INIT_SIZE);
	stackTop = -1;
	currMaxSize = STACK_INIT_SIZE;
	token t;
	t.tid = program;
	t.numVal = NULL;
	t.lexeme = NULL;
	t.lineNo = -1;
	pushToken(t);
	root = stack[stackTop];
	root->rhsUsed = -1;
}

void generateParseTree(grammar g) {
	numParseTreeNodes = 0;
	parseTreeMemUsage = 0;
	initializeStack();
	token t = getNextToken();
	treeNode *top;
	bool isSuccessful = true;
	while(stackTop >= 0) {
		//right now t is the token which is used for deciding whats gonna happen next
		if(t.tid == LEX_ERROR) {
			isSuccessful = false;
			t = getNextToken();
			continue;
		}
		top = stack[stackTop];
		if(top->t.tid >= TK_ASSIGNOP) {
			//top is a terminal
			if(t.tid == top->t.tid) {
				//matching terminals
				top->t = t;
				popToken();
				t = getNextToken();
			}
			else {
				//terminals not matching
				//print error, pop and continue
				isSuccessful = false;
				fprintf(stdout,"\nLine:%d: SYNTAX ERROR\n",t.lineNo);
				fprintf(stdout, "\tStack top: %s\n", enumToStr(top->t.tid));
				fprintf(stdout,"\tExpected token:%s.  Received token:%s  Lexeme:%s\n", enumToStr(top->t.tid), enumToStr(t.tid),t.lexeme);
				popToken();
			}
		}
		else {
			//top is a non-terminal
			if (parseTable[top->t.tid][t.tid-eps] == SKIP) {
				//not an apt token
				//print error, skip this, and continue
				isSuccessful = false;
				fprintf(stdout,"\nLine:%d: SYNTAX ERROR (SKIP)\n", t.lineNo);
				fprintf(stdout, "\tStack top: %s\n", enumToStr(top->t.tid));
				fprintf(stdout, "\tUnexpected token:%s  Lexeme:%s\n",enumToStr(t.tid),t.lexeme );
				// If we reach the EOF, then start/continue emptying that stack
				if(t.tid != SENTINEL) {
					t = getNextToken();
				}
				else {
					popToken();
				}
			}
			else if (parseTable[top->t.tid][t.tid-eps] == SYNC) {
				//pop due to sync
				//print error, but this token needs to be parsed
				isSuccessful = false;
				fprintf(stdout,"\nLine:%d: SYNTAX_ERROR (SYNC)\n",t.lineNo);
				fprintf(stdout, "\tStack top: %s\n", enumToStr(top->t.tid));
				fprintf(stdout, "\tUnexpected token:%s  Lexeme:%s\n",enumToStr(t.tid),t.lexeme );
				popToken();
			}
			else {
				//pushing the RHS starting from the right
				//excpet if it's a NULL production
				//in that case, make the apt tree node, and continue parsing
				popToken();
				int ruleNum = parseTable[top->t.tid][t.tid-eps];
				top->rhsUsed = ruleNum;
				if(g.rules[top->t.tid].rhs[ruleNum][0].name == eps) {
					token add;
					add.tid = eps;
					add.numVal = NULL;
					add.lineNo = -1;
					add.lexeme = "";
					treeNode *nodeAdd = createNode(add);
					addChild(top,nodeAdd);
				}
				else {
					for(int i = g.rules[top->t.tid].sizeRHS[ruleNum] - 1; i >= 0; i--) {
						token add;
						add.tid = g.rules[top->t.tid].rhs[ruleNum][i].name;
						// These will be initialized for terminals when they are matched
						add.lineNo = -1;
						add.lexeme = NULL;
						pushToken(add);
						addChild(top,stack[stackTop]);
					}
				}
			}
		}
	}
	if(t.tid == SENTINEL && isSuccessful) {
		printf("File successfully parsed\n");
	}
	else if (t.tid != SENTINEL) {
		hasParsingError = true;
		fprintf(stdout, "\nLine:%d: SYNTAX ERROR\n", t.lineNo);
		fprintf(stdout, "\tUnexpected token:%s  Lexeme:%s\n", enumToStr(t.tid), t.lexeme);
	}
	else if(isSuccessful == false) {
		hasParsingError = true;
	}
}

treeNode * getParseTree() {
	return root;
}

int getAndPrintParseTreeMemoryUsage(){
	printf("\nParse tree:\nNumber of nodes = %d\nAllocated Memory = %d Bytes\n",numParseTreeNodes,parseTreeMemUsage);
	return parseTreeMemUsage;
}
