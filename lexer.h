// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P

#ifndef _LEXER
#define _LEXER

#include "common_defs.h"

typedef struct _token {
	gterm tid;
	char *lexeme;
	void *numVal; //for storing int or float value for nums
	int lineNo; // for printing errors
} token;

void initLexer(char *fileName);
void cleanup();

//called by syntax analyser
token getNextToken();

#endif
