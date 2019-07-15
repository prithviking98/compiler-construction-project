// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "common_defs.h"
#include "code_gen.h"
#include "ast.h"
#include "semantic_analyser.h"

#define BUFF_SIZE 100

asmStruct outAsm;

void genAsmOtherStmts(astNode *otherStmtsNode, FILE *fptr);

void initAsm() {
	for (int i = 0; i <= MAX_TEMP_WIDTH; ++i) {
		outAsm.tempCnt[i] = outAsm.currTempCnt[i] = 0;
	}
	outAsm.labelCnt = 0;
}

void getTemp(char *buff, int width) {
	++outAsm.currTempCnt[width];
	sprintf(buff, "temp%d + %d", width, (outAsm.currTempCnt[width] - 1) * width);
	if (outAsm.currTempCnt[width] > outAsm.tempCnt[width]) {
		++outAsm.tempCnt[width];
	}
}

void resetTemps() {
	for (int i = 0; i <= MAX_TEMP_WIDTH; ++i) {
		outAsm.currTempCnt[i] = 0;
	}
}

void getLabel(char *buff) {
	++outAsm.labelCnt;
	sprintf(buff, "label%d", outAsm.labelCnt);
}

// Pass an allocated buffer, name is filled by child
void genArithExpr(astNode *arithNode, char *result, FILE *fptr) {
	if (arithNode->tag == TK_NUM) {
		fprintf(fptr, "\tmov\tr8w, %s\n", arithNode->t.lexeme);
		getTemp(result, 2);
		fprintf(fptr, "\tmov\t[%s], r8w\n", result);
		return;
	}
	else if (arithNode->tag == TK_RNUM) {
		assert(0);
		return;
	}
	else if (arithNode->tag == all) {
		astNode *child = arithNode->firstChild;
		if (child->nextSibling == NULL) {
			// Need to store the lexeme in a temp since it might
			// be overwritten
			fprintf(fptr, "\tmov\tr8w, [%s]\n", child->t.lexeme);
		}
		else {
			functionTableEntry *fte = searchInFT(ft, "_main");
			idTableEntry *ite = searchInIDT(fte->lidt, child->t.lexeme);
			if (ite == NULL) {
				ite = searchInIDT(gidt, child->t.lexeme);
			}
			fprintf(fptr, "\tmov\tr8w, [%s + __%s.%s]\n",
				child->t.lexeme, ite->rptr->key + 1, child->nextSibling->t.lexeme);
		}
		getTemp(result, 2); // Numbers are 2 bytes
		fprintf(fptr, "\tmov\t[%s], r8w\n", result);
		return;
	}
	char *leftRes, *rightRes;
	// reusing left result for our result
	leftRes = result;
	rightRes = malloc(BUFF_SIZE * sizeof(char));
	astNode *leftChild = arithNode->firstChild;
	astNode *rightChild = leftChild->nextSibling;
	genArithExpr(leftChild, leftRes, fptr);
	genArithExpr(rightChild, rightRes, fptr);
	fprintf(fptr, "\tmov\tr8w, [%s]\n", rightRes);
	if (arithNode->tag == TK_PLUS) {
		fprintf(fptr, "\tadd\t[%s], r8w\n", result);
	}
	else if (arithNode->tag == TK_MINUS) {
		fprintf(fptr, "\tsub\t[%s], r8w\n", result);
	}
	else if (arithNode->tag == TK_MUL) {
		fprintf(fptr, "\timul\tr8w, [%s]\n", leftRes);
		fprintf(fptr, "\tmov\t[%s], r8w\n", result);
	}
	else {
		fprintf(fptr, "\tmov\tax, [%s]\n", leftRes);
		fprintf(fptr, "\tmov\tdx, 0\n");
		fprintf(fptr, "\tmov\tr8w, [%s]\n", rightRes);
		fprintf(fptr, "\tidiv\tr8w\n");
		// Q <- ax, R <- dx
		fprintf(fptr, "\tmov\t[%s], ax\n", result);
	}
	free(rightRes);
}

void genAssignStmt(astNode *assignNode, FILE *fptr) {
	char *rhs = malloc(BUFF_SIZE * sizeof(char));
	astNode *lhsNode = assignNode->firstChild;
	astNode *rhsNode = lhsNode->nextSibling;
	genArithExpr(rhsNode, rhs, fptr);
	fprintf(fptr, "\tmov\tr8w, [%s]\n", rhs);
	if (lhsNode->firstChild->nextSibling == NULL) {
		fprintf(fptr, "\tmov\t[%s], r8w\n", lhsNode->firstChild->t.lexeme);
	}
	else {
		astNode *temp = lhsNode->firstChild;
		functionTableEntry *fte = searchInFT(ft, "_main");
		idTableEntry *ite = searchInIDT(fte->lidt, temp->t.lexeme);
		if (ite == NULL) {
			ite = searchInIDT(gidt, temp->t.lexeme);
		}
		fprintf(fptr, "\tmov\t[%s + __%s.%s], r8w\n",
			temp->t.lexeme, ite->rptr->key + 1, temp->nextSibling->t.lexeme);
	}
	free(rhs);
}

void genBoolExpr(astNode *boolNode, char *result, FILE *fptr) {
	// If it's an operand (TK_ID, TK_NUM, TK_RNUM)
	if (boolNode->firstChild == NULL) {
		if (boolNode->tag == TK_ID) {
			fprintf(fptr, "\tmov\tr8w, [%s]\n", boolNode->t.lexeme);
		}
		else if (boolNode->tag == TK_NUM) {
			fprintf(fptr, "\tmov\tr8w, %s\n", boolNode->t.lexeme);
		}
		else {
			assert(0);
		}
		getTemp(result, 2);
		fprintf(fptr, "\tmov\t[%s], r8w\n", result);
		return;
	}
	astNode *leftChild = boolNode->firstChild;
	astNode *rightChild = leftChild->nextSibling;
	char *leftRes, *rightRes;
	leftRes = result;
	genBoolExpr(leftChild, leftRes, fptr);
	if (boolNode->tag == TK_NOT) {
		fprintf(fptr, "\txor\t[%s], 1\n", leftRes);
		return;
	}
	rightRes = malloc(BUFF_SIZE * sizeof(char));

	// Note the reg sizes
	genBoolExpr(rightChild, rightRes, fptr);
	if (boolNode->tag == TK_AND) {
		fprintf(fptr, "\tmov\tr8b, [%s]\n", rightRes);
		fprintf(fptr, "\tand\t[%s], r8b\n", leftRes);
		free(rightRes);
		return;
	}
	else if (boolNode->tag == TK_OR) {
		fprintf(fptr, "\tmov\tr8b, [%s]\n", rightRes);
		fprintf(fptr, "\tor\t[%s], r8b\n", leftRes);
		free(rightRes);
		return;
	}

	// For rel ops, the children's results are 2 bytes
	// While the rel op's result is 1 byte
	char *leftResTemp = malloc(BUFF_SIZE * sizeof(char));
	strcpy(leftResTemp, leftRes);
	leftRes = leftResTemp;
	fprintf(fptr, "\tmov\tr8w, [%s]\n", rightRes);
	fprintf(fptr, "\tcmp\t[%s], r8w\n", leftRes);
	if (boolNode->tag == TK_LT) {
		fprintf(fptr, "\tsetlt\tr9b\n");
	}
	else if (boolNode->tag == TK_LE) {
		fprintf(fptr, "\tsetle\tr9b\n");
	}
	else if (boolNode->tag == TK_EQ) {
		fprintf(fptr, "\tsete\tr9b\n");
	}
	else if (boolNode->tag == TK_GT) {
		fprintf(fptr, "\tsetgt\tr9b\n");
	}
	else if (boolNode->tag == TK_GE) {
		fprintf(fptr, "\tsetge\tr9b\n");
	}
	else if (boolNode->tag == TK_NE) {
		fprintf(fptr, "\tsetne\tr9b\n");
	}
	else {
		assert(0);
	}
	getTemp(result, 1);
	fprintf(fptr, "\tmov\t[%s], r9b\n", result);
	free(leftResTemp);
	free(rightRes);
}

void genIterStmt(astNode *iterNode, FILE *fptr) {
	astNode *boolNode = iterNode->firstChild;
	astNode *bodyNode = boolNode->nextSibling;
	char *result = malloc(BUFF_SIZE * sizeof(char));
	char *label1 = malloc(BUFF_SIZE * sizeof(char));
	getLabel(label1);
	char *label2 = malloc(BUFF_SIZE * sizeof(char));
	getLabel(label2);

	fprintf(fptr, "%s:\n", label1);
	genBoolExpr(boolNode, result, fptr);
	fprintf(fptr, "\tcmp\t[%s], byte 1\n", result);
	fprintf(fptr, "\tjne\t%s\n", label2);
	genAsmOtherStmts(bodyNode, fptr);
	fprintf(fptr, "\tjmp\t%s\n", label1);
	fprintf(fptr, "%s:\n", label2);

	free(label1);
	free(label2);
	free(result);
}

void genCondStmt(astNode *condNode, FILE *fptr) {
	astNode *boolNode = condNode->firstChild;
	astNode *ifNode = boolNode->nextSibling;
	astNode *elseNode = ifNode->nextSibling;
	char *result = malloc(BUFF_SIZE * sizeof(char));
	char *label1 = malloc(BUFF_SIZE * sizeof(char));
	getLabel(label1);
	char *label2 = malloc(BUFF_SIZE * sizeof(char));
	getLabel(label2);

	genBoolExpr(boolNode, result, fptr);
	fprintf(fptr, "\tcmp\t[%s], byte 1\n", result);
	fprintf(fptr, "\tjne\t%s\n", label1);
	genAsmOtherStmts(ifNode, fptr);
	fprintf(fptr, "\tjmp\t%s\n", label2);
	fprintf(fptr, "%s:\n", label1);
	genAsmOtherStmts(elseNode, fptr);
	fprintf(fptr, "%s:\n", label2);

	free(label1);
	free(label2);
	free(result);
}

void genWriteStmt(astNode *writeNode, FILE *fptr) {
	astNode *child = writeNode->firstChild;
	fprintf(fptr, "\tmov\trdi, intwrite\n");
	if (child->tag == TK_NUM) {
		fprintf(fptr, "\tmov\trsi, %s\n", child->t.lexeme);
	}
	else if (child->tag == TK_RNUM) {
		assert(0);
	}
	else {
		child = child->firstChild;
		if (child->nextSibling == NULL) {
			fprintf(fptr, "\tmov\trsi, [%s]\n", child->t.lexeme);
		}
		else {
			functionTableEntry *fte = searchInFT(ft, "_main");
			idTableEntry *ite = searchInIDT(fte->lidt, child->t.lexeme);
			if (ite == NULL) {
				ite = searchInIDT(gidt, child->t.lexeme);
			}
			fprintf(fptr, "\tmov\trsi, [%s + __%s.%s]\n",
				child->t.lexeme, ite->rptr->key + 1, child->nextSibling->t.lexeme);
		}
	}
	fprintf(fptr, "\tmov\trax, 0\n");
	fprintf(fptr, "\tcall\tprintf\n");
}

void genReadStmt(astNode *readNode, FILE *fptr) {
	astNode *child = readNode->firstChild->firstChild;
	fprintf(fptr, "\tmov\trdi, intread\n");
	if (child->nextSibling == NULL) {
		fprintf(fptr, "\tmov\trsi, %s\n", child->t.lexeme);
	}
	else {
		functionTableEntry *fte = searchInFT(ft, "_main");
		idTableEntry *ite = searchInIDT(fte->lidt, child->t.lexeme);
		if (ite == NULL) {
			ite = searchInIDT(gidt, child->t.lexeme);
		}
		fprintf(fptr, "\tmov\trsi, %s + __%s.%s\n",
			child->t.lexeme, ite->rptr->key + 1, child->nextSibling->t.lexeme);
	}
	fprintf(fptr, "\tmov\trax, 0\n");
	fprintf(fptr, "\tcall\tscanf\n");
}

void genAsmOtherStmts(astNode *otherStmtsNode, FILE *fptr) {
	astNode *curr = otherStmtsNode->firstChild;
	while (curr != NULL) {
		switch(curr->tag) {
		case assignmentStmt:
			genAssignStmt(curr, fptr);
			break;
		case iterativeStmt:
			genIterStmt(curr, fptr);
			break;
		case conditionalStmt:
			genCondStmt(curr, fptr);
			break;
		case TK_WRITE:
			genWriteStmt(curr, fptr);
			break;
		case TK_READ:
			genReadStmt(curr, fptr);
			break;
		default:
			assert(0);
		}
		resetTemps();
		curr = curr->nextSibling;
	}
}

void genAsmDecls(astNode *declsNode, FILE *fptr) {
	astNode *curr = declsNode->firstChild;
	while (curr != NULL) {
		astNode *typeNode = curr->firstChild;
		astNode *nameNode = typeNode->nextSibling;
		if (typeNode->tag == TK_INT) {
			fprintf(fptr, "\t%s:\tresb\t2\n", nameNode->t.lexeme);
		}
		else if (typeNode->tag == TK_REAL) {
			fprintf(fptr, "\t%s:\tresb\t4\n", nameNode->t.lexeme);
		}
		else {
			char *buffer = malloc(BUFF_SIZE * sizeof(char));
			strcpy(buffer, typeNode->t.lexeme + 1);
			fprintf(fptr, "\t%s:\tresb\t__%s_size\n", nameNode->t.lexeme, buffer);
			free(buffer);
		}
		curr = curr->nextSibling;
	}
	int i;
	for (i = 0; i <= MAX_TEMP_WIDTH; ++i) {
		if (outAsm.tempCnt[i] == 0) {
			continue;
		}
		fprintf(fptr, "\ttemp%d:\tresb\t%d\n", i, outAsm.tempCnt[i] * i);
	}
}

void genAsmTypeDefs(astNode *typeDefsNode, FILE *fptr) {
	astNode *currRec = typeDefsNode->firstChild;
	while (currRec != NULL) {
		astNode *recName = currRec->firstChild;
		char *buffer = malloc(BUFF_SIZE * sizeof(char));
		strcpy(buffer, recName->t.lexeme + 1);
		fprintf(fptr, "struc __%s\n", buffer);
		free(buffer);
		astNode *currField = recName->nextSibling->firstChild;
		while (currField != NULL) {
			if (currField->tag == TK_INT) {
				fprintf(fptr, "\t.%s:\tresb\t2\n", currField->nextSibling->t.lexeme);
			}
			else {
				fprintf(fptr, "\t.%s:\tresb\t4\n", currField->nextSibling->t.lexeme);
			}
			currField = currField->nextSibling->nextSibling;
		}
		fprintf(fptr, "endstruc\n");
		currRec = currRec->nextSibling;
	}
}

void generateAsm(astNode *root, FILE *fptr) {
	assert(root->firstChild->tag == mainFunction);
	initAsm();
	astNode *stmtsNode = root->firstChild->firstChild;
	astNode *typeDefsNode = stmtsNode->firstChild;
	astNode *declsNode = typeDefsNode->nextSibling;
	astNode *otherStmtsNode = declsNode->nextSibling;
	// astNode *retNode = otherStmtsNode->nextSibling;

	genAsmTypeDefs(typeDefsNode, fptr);

	fprintf(fptr, "section .data\n");
	fprintf(fptr, "\tintwrite:\tdb\t'%%hd',10,0\n");
	fprintf(fptr, "\tintread:\tdb\t'%%hd',0\n");
	fprintf(fptr, "\n");

	fprintf(fptr, "section .text\n");
	fprintf(fptr, "\tglobal main\n");
	fprintf(fptr, "\textern printf\n");
	fprintf(fptr, "\textern scanf\n");
	fprintf(fptr, "\n");

	// TODO Stack pointer for printf scanf
	fprintf(fptr, "main:\n");
	fprintf(fptr, "\tsub\trsp, 8\n");
	fprintf(fptr, "\n");
	genAsmOtherStmts(otherStmtsNode, fptr);
	fprintf(fptr, "\n");
	fprintf(fptr, "\tadd\trsp, 8\n");
	fprintf(fptr, "\tret\n");
	fprintf(fptr, "\n");

	fprintf(fptr, "section .bss\n");
	// Allocates Temps
	genAsmDecls(declsNode, fptr);
}
