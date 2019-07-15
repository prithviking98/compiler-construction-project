// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P
#ifndef _SEMANTIC_ANALYSER
#define _SEMANTIC_ANALYSER

#include "common_defs.h"
#include "table_id.h"
#include "table_func.h"
#include "table_rec.h"
#include "ast.h"
#include <stdbool.h>

extern idTable gidt;
extern functionTable ft;
extern recordTable rt;
extern bool hasSemanticError;

void initSemanticAnalyser();

void semanticAnalyser(astNode *ast); //return true if no errors

void printAllVariables(astNode *ast);

#endif
