// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#ifndef _PARSER_PREPROCESS
#define _PARSER_PREPROCESS

#include "common_defs.h" // TODO: is this needed?
#include<stdbool.h>
#include<stdio.h>

#define MASK_eps (1LL << 0)
#define MASK_dollar (1LL << 0)

#define SYNC -1
#define SKIP -2

typedef long long ll;

typedef struct _gSymbol {
	bool isTerminal;
	gterm name;
} gSymbol;

typedef struct _rule {
	gSymbol lhs;
	gSymbol **rhs;
	int numRHS;
	int *sizeRHS;
} rule;

typedef struct _grammar {
	rule *rules;
	int numLHS;
	long long* first;
	long long* follow;
} grammar;

/* reads file in fptr and constructs grammar */
grammar readGrammar(FILE *fptr);
void printGrammar(grammar g);
void printFirst(grammar g);
void printFollow(grammar g);
ll calcFirstSetSentential(grammar g, int lhsNum, int rhsNum, int startNum);
void calcParseTable(grammar g);
//doesn't return anything, outputs parse table matrix to a file

#endif
/*#define MASK_TK_ASSIGNOP (1LL << 1);
#define MASK_TK_COMMENT (1LL << 2);
#define MASK_TK_FIELDID (1LL << 3);
#define MASK_TK_ID (1LL << 4);
#define MASK_TK_NUM (1LL << 5);
#define MASK_TK_RNUM (1LL << 6);
#define MASK_TK_FUNID (1LL << 7);
#define MASK_TK_RECORDID (1LL << 8);
#define MASK_TK_WITH (1LL << 9);
#define MASK_TK_PARAMETERS (1LL << 10);
#define MASK_TK_END (1LL << 11);
#define MASK_TK_WHILE (1LL << 12);
#define MASK_TK_TYPE (1LL << 13);
#define MASK_TK_MAIN (1LL << 14);
#define MASK_TK_GLOBAL (1LL << 15);
#define MASK_TK_PARAMETER (1LL << 16);
#define MASK_TK_LIST (1LL << 17);
#define MASK_TK_SQL (1LL << 18);
#define MASK_TK_SQR (1LL << 19);
#define MASK_TK_INPUT (1LL << 20);
#define MASK_TK_OUTPUT (1LL << 21);
#define MASK_TK_INT (1LL << 22);
#define MASK_TK_REAL (1LL << 23);
#define MASK_TK_COMMA (1LL << 24);
#define MASK_TK_SEM (1LL << 25);
#define MASK_TK_COLON (1LL << 26);
#define MASK_TK_DOT (1LL << 27);
#define MASK_TK_ENDWHILE (1LL << 28);
#define MASK_TK_OP (1LL << 29);
#define MASK_TK_CL (1LL << 30);
#define MASK_TK_IF (1LL << 31);
#define MASK_TK_THEN (1LL << 32);
#define MASK_TK_ENDIF (1LL << 33);
#define MASK_TK_READ (1LL << 34);
#define MASK_TK_WRITE (1LL << 35);
#define MASK_TK_RETURN (1LL << 36);
#define MASK_TK_PLUS (1LL << 37);
#define MASK_TK_MINUS (1LL << 38);
#define MASK_TK_MUL (1LL << 39);
#define MASK_TK_DIV (1LL << 40);
#define MASK_TK_CALL (1LL << 41);
#define MASK_TK_RECORD (1LL << 42);
#define MASK_TK_ENDRECORD (1LL << 43);
#define MASK_TK_ELSE (1LL << 44);
#define MASK_TK_AND (1LL << 45);
#define MASK_TK_OR (1LL << 46);
#define MASK_TK_NOT (1LL << 47);
#define MASK_TK_LT (1LL << 48);
#define MASK_TK_LE (1LL << 49);
#define MASK_TK_EQ (1LL << 50);
#define MASK_TK_GT (1LL << 51);
#define MASK_TK_GE (1LL << 52);
#define MASK_TK_NE (1LL << 53);*/
