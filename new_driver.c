// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P

#include <stdio.h>
#include <time.h>
#include "common_defs.h"
#include "lexer.h"
#include "parser_preprocess.h"
#include "parser.h"
#include "ast.h"
#include "type_extraction.h"
#include "table_id.h"
#include "table_rec.h"
#include "table_func.h"
#include "semantic_analyser.h"
#include "code_gen.h"


void parse(char* inputFile);
void constructAST();
void checkSemantics();
void printTokens(char *file);
void printLocalVariabls();

grammar g;
double parsing_time, ast_gen_time, semantic_checking_time;
double parsing_clicks, ast_gen_clicks, semantic_checking_clicks;
astNode *AST = NULL;
int parsingCompleted = 0 ;
int ASTGenerated = 0;
int semanticsChecked = 0;


int main(int argc,char* argv[]) {
	if(argc != 3){
		printf("Invalid number of arguments: please specify command as\n");
		printf("./compiler testcase.txt code.asm\n");
		return 0;
	}
	g=readGrammar(fopen("grammar.txt","r"));
	calcParseTable(g);
	int op;
	char *inputFile=argv[1];
	char *asmCodeOutFile=argv[2];
	//Add any initial comments to prin
	printf("\n====Implementation Details====\n");
	printf("1) First and Follow set calculation has been automated\n");
	printf("2) Both lexical and syntax analysis modules implemented\n");
	printf("3) Parse tree construction completed\n");
	printf("4) AST construction completed\n");
	printf("5) Semantic rules checking completed\n");
	printf("6) Type checking completed\n");
	printf("7) Level of compiler: 4 (all error checks except for while loop variable updation have been implemented, including some additional errors not specified\n\n");
	while(1){
		printf("================Options================\n");
		printf("0 : Exit\n");
		printf("1 : Print Token List\n");
		printf("2 : Parse and Print Parse Tree\n");
		printf("3 : Print AST\n");
		printf("4 : Display Parse Tree and AST Memory Usage\n");
		printf("5 : Display Symbol Tables\n");
		printf("6 : Display Global Variables\n");
		printf("7 : Display Function Memory Requirements\n");
		printf("8 : Display Record Definitions\n");
		printf("9 : Verify Syntactical and Semantic Correctness (print time)\n");
		printf("10: Compile and generate assembly code\n");
		printf("Enter option: ");
		scanf("%d",&op);
		if(op==0)
			break;
		else if(op==1){
			//Print Token List
			printf("\n=============Tokens=============\n");
			printTokens(inputFile);
			printf("\n==============END==============\n\n");
		}
		else if(op==2){
			// Parse and Print Parse Tree
			parse(inputFile);
			printf("\n==============Parsing==============\n");
			if(hasParsingError) {
				printf("\n\nCode has syntax errors. Unable to print parse tree.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			printParseTree(stdout);
			printf("\n==============END==============\n\n");
		}
		else if(op==3){
			// Print AST
			parse(inputFile);
			printf("\n===============AST==================\n");
			printf("| Node |============| Parent |===========|Lex Val(If Leaf)|\n");
			constructAST();
			printAST(AST);
			printf("\n\nThe AST has been printed in a depth first manner.\nFor Easier Reference Parent of each Node is also displayed alongside\n");
		}
		else if(op==4){
			//Display Parse Tree and AST Memory Usage
			int parseTreeMem,ASTMem;
			parse(inputFile);
			printf("\n========Memory Usage===========\n");
			if(hasParsingError) {
				printf("\n\nCode has syntax errors. Unable to find memory usage and calculate AST.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			parseTreeMem = getAndPrintParseTreeMemoryUsage();
			constructAST();
			ASTMem = getAndPrintASTMemoryUsage();
			printf("\nCompression Percentage = %f",100*((float)(parseTreeMem-ASTMem)/(float)parseTreeMem));
			printf("\n==============END==============\n\n");
		}
		else if(op==5){
			//print all variables
			printf("\n==============All Variables==============\n");
			parse(inputFile);
			if(hasParsingError) {
				printf("\n\nCode has syntax errors. Unable to calculate AST.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			constructAST();
			checkSemantics();
			if(hasSemanticError) {
				printf("\n\nCode has semantic errors. Unable to proceed.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			printAllVariables(AST);
			printf("\n==============END==============\n\n");
		}
		else if(op==6){
			//Display Global Variables
			parse(inputFile);
			printf("\n==============Global Variables==============\n");
			if(hasParsingError) {
				printf("\n\nCode has syntax errors. Unable to calculate AST.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			constructAST();
			checkSemantics();
			if(hasSemanticError) {
				printf("\n\nCode has semantic errors. Unable to proceed.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			printGlobalVarList();
			printf("\n==============END==============\n\n");
		}
		else if(op==7){
			//Display Function Memory Requirements
			parse(inputFile);
			printf("\n==============Functions==============\n");
			if(hasParsingError) {
				printf("\n\nCode has syntax errors. Unable to proceed.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			constructAST();
			checkSemantics();
			if(hasSemanticError) {
				printf("\n\nCode has semantic errors. Unable to proceed.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			printFuncs(AST);
			printf("\n==============END==============\n\n");
		}
		else if(op==8){
			//Display Record Definitions
			parse(inputFile);
			printf("\n==============Record Types==============\n");
			if(hasParsingError) {
				printf("\n\nCode has syntax errors. Unable to proceed.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			constructAST();
			checkSemantics();
			if(hasSemanticError) {
				printf("\n\nCode has semantic errors. Unable to proceed.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			printExtractedRecs();
		}
		else if(op==9){
			//Verify Syntactical and Semantic Correctness
			parse(inputFile);
			printf("\n============Verifying Syntax and Semantics===========\n");
			if(hasParsingError) {
				printf("\n\nCode has syntax errors. Unable to proceed.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			constructAST();
			checkSemantics();
			if(hasSemanticError) {
				printf("\n\nCode has semantic errors. Unable to proceed.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			printf("\n\nSemantic errors checking completed ... \n\n");
			double total_clicks = parsing_clicks + ast_gen_clicks + semantic_checking_clicks;
			double total_time = parsing_time + ast_gen_time + semantic_checking_time;
			printf("\ntotal_CPU_time = %lf",total_clicks);
			printf("\ntotal_CPU_time_in_seconds = %lf \n",total_time);
			printf("\n==============END==============\n\n");
		}
		else if(op==10){
			parse(inputFile);
			printf("\n============Compiling and generating assembly code===========\n");
			if(hasParsingError) {
				printf("\n\nCode has syntax errors. Unable to proceed.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			constructAST();
			checkSemantics();
			if(hasSemanticError) {
				printf("\n\nCode has semantic errors. Unable to proceed.\n");
				printf("\n==============END==============\n\n");
				continue;
			}
			printf("\n\nSemantic errors checking completed ... \n");
			FILE *asmFile = fopen(asmCodeOutFile,"w");
			generateAsm(AST,asmFile);
			fclose(asmFile);
			printf("\n\nAssembly code generated\n");
			printf("\n==============END==============\n\n");
		}
		else {
			printf("\nIncorrect option. Please try again");

		}
	}
}

void parse(char* inputFile){
	if(!parsingCompleted){
		clock_t start_time, end_time;
		start_time = clock();

		initLexer(inputFile);
		generateParseTree(g);
		cleanup();

		end_time = clock();
		parsing_clicks = (double)(end_time - start_time);
		parsing_time = parsing_clicks/CLOCKS_PER_SEC;
		parsingCompleted=1;
	}
}

void constructAST(){
	if(!ASTGenerated){
		clock_t start_time, end_time;
		start_time = clock();

		AST=generateAST();
		end_time = clock();

		ast_gen_clicks = (double)(end_time - start_time);
		ast_gen_time = ast_gen_time/CLOCKS_PER_SEC;
		ASTGenerated = 1;
	}
}

void checkSemantics(){
	if(!semanticsChecked){
		clock_t start_time, end_time;
		start_time = clock();

		initSemanticAnalyser();
		semanticAnalyser(AST);

		end_time = clock();
		semantic_checking_clicks = (double)(end_time - start_time);
		semantic_checking_time = semantic_checking_clicks/CLOCKS_PER_SEC;
		semanticsChecked = 1;
	}
}

void printTokens(char *file) {
	initLexer(file);
	token t;
	while(1) {
		t=getNextToken();
		if(t.tid==SENTINEL)
			break;
		else if(t.tid==LEX_ERROR)
			continue; //if error token, continue;
		printf("\nLine:%d\tToken:%s\t",t.lineNo,enumToStr(t.tid));
		printf("Lexeme:%s\t",t.lexeme);

		if(t.numVal!=NULL){
			if(t.tid==TK_NUM)
				printf("IntValue:%d",*((int*)t.numVal));
			else
				printf("RealValue:%lf",*((double*)t.numVal));
		}
	}
	cleanup();
}

