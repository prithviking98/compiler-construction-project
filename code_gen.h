// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P
#ifndef _CODE_GEN
#define _CODE_GEN

#include <stdio.h>
#include "ast.h"

#define MAX_TEMP_WIDTH 8

typedef struct _asmStruct {
	int tempCnt[MAX_TEMP_WIDTH + 1], currTempCnt[MAX_TEMP_WIDTH + 1];
	int labelCnt;
} asmStruct;

void generateAsm(astNode *root, FILE *fptr);

#endif
