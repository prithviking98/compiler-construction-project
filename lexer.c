// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "common_defs.h"
#include "lexer.h"

int line;//line no. in file

typedef struct _keyword {
	char key[15];
	gterm tid;
}keyword;

#define NUM_KEYWORDS 23
keyword keywords[] = {
	{"call", TK_CALL},
	{"else", TK_ELSE},
	{"end", TK_END},
	{"endif", TK_ENDIF},
	{"endrecord", TK_ENDRECORD},
	{"endwhile", TK_ENDWHILE},
	{"global", TK_GLOBAL},
	{"if", TK_IF},
	{"input", TK_INPUT},
	{"int", TK_INT},
	{"list", TK_LIST},
	{"output", TK_OUTPUT},
	{"parameter", TK_PARAMETER},
	{"parameters", TK_PARAMETERS},
	{"read", TK_READ},
	{"real", TK_REAL},
	{"record", TK_RECORD},
	{"return", TK_RETURN},
	{"then", TK_THEN},
	{"type", TK_TYPE},
	{"while", TK_WHILE},
	{"with", TK_WITH},
	{"write", TK_WRITE}
};

FILE *fptr;
// The size of a lexeme can't exceed BUFFER_SIZE
#define BUFFER_SIZE (1 << 12)
char buff1[BUFFER_SIZE + 1];
char buff2[BUFFER_SIZE + 1];
// forward represents the lookahead pointer
char *lexemeBegin, *forward;
// Used to make sure we don't fread again after retracting past the edge of a buffer
bool read1, read2;
// Keeps track of the size of the current lexeme for efficiency
int lexemeSize;

void initLexer(char *fileName) {
	line = 1;
	fptr = fopen(fileName, "r");
	assert(fptr != NULL);
	int size1 = fread(buff1, sizeof(char), BUFFER_SIZE, fptr);
	buff1[size1] = EOF;
	lexemeBegin = forward = buff1;
	read1 = read2 = true;
	lexemeSize = 0;
}

void cleanup() {
	fclose(fptr);
}

// Get the next character from the input stream
char getNextChar() {
	char curr = *forward;
	if (forward == buff1 + BUFFER_SIZE - 1) {
		if (read2) {
			int size2 = fread(buff2, sizeof(char), BUFFER_SIZE, fptr);
			buff2[size2] = EOF;
		}
		forward = buff2;
		read2 = true; // Resetting the flag if it was false
	}
	else if (forward == buff2 + BUFFER_SIZE - 1) {
		if (read1) {
			int size1 = fread(buff1, sizeof(char), BUFFER_SIZE, fptr);
			buff1[size1] = EOF;
		}
		forward = buff1;
		read1 = true; // Resetting the flag if it was false
	}
	else {
		++forward;
	}
	++lexemeSize;
	return curr;
}

// int n: how many characters to retract
void retract(int n) {
	if (forward >= buff1 && forward < buff1 + BUFFER_SIZE) {
		if (forward - buff1 < n) {
			read1 = false; // Don't load buff1 the next time
			n -= forward - buff1;
			forward = buff2 + BUFFER_SIZE - n;
		}
		else {
			forward -= n;
		}
	}
	else {
		if (forward - buff2 < n) {
			read2 = false; // Don't load buff2 the next time
			n -= forward - buff2;
			forward = buff1 + BUFFER_SIZE - n;
		}
		else {
			forward -= n;
		}
	}
	lexemeSize -= n;
}

// Returns the lexeme just read (null-terminated)
char * getLexeme() {
	char *lexeme = (char *)malloc((lexemeSize + 1) * sizeof(char));
	int i = 0;
	char *currptr = lexemeBegin;
	while (currptr != forward) {
		lexeme[i] = *currptr;
		++i;
		if (currptr == buff1 + BUFFER_SIZE - 1) {
			currptr = buff2;
		}
		else if (currptr == buff2 + BUFFER_SIZE - 1) {
			currptr = buff1;
		}
		else {
			++currptr;
		}
	}
	lexeme[i] = '\0';
	return lexeme;
}

// Reset to start reading a new lexeme
void reset() {
	lexemeBegin = forward;
	lexemeSize = 0;
}


gterm isKeyword(char *lexeme) {
	int l = 0, r = NUM_KEYWORDS - 1;
	while (l <= r) {
		int mid = (l + r) / 2;
		int ret = strcmp(lexeme, keywords[mid].key);
		if (ret == 0) {
			return keywords[mid].tid;
		}
		else if (ret < 0) {
			r = mid - 1;
		}
		else {
			l = mid + 1;
		}
	}
	return TK_FIELDID;
}

token getNextToken() {
	int state=0;
	int err=-1; //error state
	int idLen=0; //to count identifier lengths
	char c;
	char *lexeme;
	token t;
	t.numVal=NULL;
	t.lexeme="";
	while(state >= 0) {
		switch(state)
		{
		case 0:
			c=getNextChar();
			// fprintf(stderr, "%c",c );
			if(c=='\n') {
				++line;
				reset();
			}
			else if(c==' '||c=='\t')
				reset();
			else if(c=='[')
				state=1;
			else if(c==']')
				state=2;
			else if(c==',')
				state=3;
			else if(c==';')
				state=4;
			else if(c==':')
				state=5;
			else if(c=='.')
				state=6;
			else if(c=='(')
				state=7;
			else if(c==')')
				state=8;
			else if(c=='+')
				state=9;
			else if(c=='-')
				state=10;
			else if(c=='*')
				state=11;
			else if(c=='/')
				state=12;
			else if(c=='~')
				state=13;
			else if(c=='!')
				state=14;
			else if(c=='&')
				state=15;
			else if(c=='@')
				state=17;
			else if(c=='=')
				state=19;
			else if(c=='<')
				state=20;
			else if(c=='>')
				state=23;
			else if(c=='%')
				state=24;
			else if(c=='#')
				state=25;
			else if(c >= '0' && c <= '9')
				state=27;
			else if(c=='_')
				state=30;
			else if(c >= 'b' && c <= 'd')
				state=34;
			else if(c == 'a' || (c >= 'e' && c <= 'z'))
				state=38;
			else if(c==EOF)
				state=40;
			else {
				state=-1;
				err=0;
			}
			break;
		case 1:
			t.tid=TK_SQL;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 2:
			t.tid=TK_SQR;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 3:
			t.tid=TK_COMMA;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 4:
			t.tid=TK_SEM;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 5:
			t.tid=TK_COLON;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 6:
			t.tid=TK_DOT;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 7:
			t.tid=TK_OP;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 8:
			t.tid=TK_CL;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 9:
			t.tid=TK_PLUS;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 10:
			t.tid=TK_MINUS;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 11:
			t.tid=TK_MUL;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 12:
			state=0;
			t.tid=TK_DIV;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 13:
			t.tid=TK_NOT;
			t.lineNo=line;
			t.lexeme=getLexeme();
			reset();
			return t;
		case 14:
			if((c=getNextChar()) == '=') {
				t.tid = TK_NE;
				t.lineNo = line;
				t.lexeme=getLexeme();
				reset();
				return t;
			}
			else {
				state=-1;
				err=14;
			}
			break;

		case 15:
			if((c=getNextChar()) == '&')
				state=16;
			else {
				state=-1;
				err=15;
			}
			break;

		case 16:
			if((c=getNextChar())=='&') {
				t.tid=TK_AND;
				t.lineNo=line;
				t.lexeme=getLexeme();
				reset();
				return t;
			}
			else {
				state=-1;
				err=16;
			}
			break;

		case 17:
			if((c=getNextChar()) == '@')
				state=18;
			else {
				state=-1;
				err=17;
			}
			break;

		case 18:
			if((c=getNextChar()) == '@') {
				t.tid=TK_OR;
				t.lineNo=line;
				t.lexeme=getLexeme();
				reset();
				return t;
			}
			else {
				state=-1;
				err=18;
			}
			break;

		case 19:
			if((c=getNextChar()) == '=') {
				t.tid=TK_EQ;
				t.lineNo=line;
				t.lexeme=getLexeme();
				reset();
				return t;
			}
			else {
				state=-1;
				err=19;
			}
			break;

		case 20:
			c=getNextChar();
			if(c=='=') {
				t.tid=TK_LE;
				t.lineNo=line;
				t.lexeme=getLexeme();
				reset();
				return t;
			}
			else if(c=='-')
				state=21;
			else {
				t.tid=TK_LT;
				t.lineNo=line;
				retract(1);
				t.lexeme=getLexeme();
				reset();
				return t;
			}
			break;

		case 21:
			if((c=getNextChar())=='-')
				state=22;
			else {
				state=-1;
				err=21;
			}
			break;

		case 22:
			if((c=getNextChar())=='-') {
				t.tid=TK_ASSIGNOP;
				t.lineNo=line;
				t.lexeme=getLexeme();
				reset();
				return t;
			}
			else {
				state=-1;
				err=22;
			}
			break;

		case 23:
			c=getNextChar();
			if(c=='=') {
				t.tid=TK_GE;
				t.lineNo=line;
				t.lexeme=getLexeme();
				reset();
				return t;
			}
			else {
				t.tid=TK_GT;
				t.lineNo=line;
				t.lexeme=getLexeme();
				reset();
				return t;
			}

		case 24:
			if(getNextChar() =='\n') {
				state=0;
				retract(1);
				reset();
			}
			break;

		case 25:
			c=getNextChar();
			if(c >=	'a' && c <= 'z')
				state=26;
			else {
				state=-1;
				err=25;
			}
			break;

		case 26:
			c=getNextChar();
			if(c >=	'a' && c <= 'z')
				state=26;
			else {
				t.tid=TK_RECORDID;
				t.lineNo=line;
				retract(1);
				lexeme=getLexeme();
				t.lexeme=lexeme;
				reset();
				return t;
			}
			break;

		case 27:
			c=getNextChar();
			if(c >= '0' && c <= '9')
				state=27;
			else if(c=='.')
				state=28;
			else {
				t.tid=TK_NUM;
				t.lineNo=line;
				retract(1);
				lexeme=getLexeme();
				t.lexeme=lexeme;
				t.numVal=malloc(sizeof(int));
				int *temp_ptr;
				temp_ptr=t.numVal;
				*(temp_ptr)=atoi(lexeme);
				reset();
				return t;
			}
			break;

		case 28:
			c=getNextChar();
			if(c >= '0' && c <= '9')
				state=29;
			else {
				state=-1;
				err=28;
			}
			break;

		case 29:
			c=getNextChar();
			if(c >= '0' && c <= '9') {
				t.tid=TK_RNUM;
				t.lineNo=line;
				lexeme=getLexeme();
				t.lexeme=lexeme;
				t.numVal=malloc(sizeof(double));
				double *temp_ptr;
				temp_ptr=t.numVal;
				sscanf(lexeme,"%lf",temp_ptr);
				reset();
				return t;
			}
			else {
				state=-1;
				err=29;
			}
			break;

		case 30:
			c=getNextChar();
			idLen=0;
			if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
				state=31;
				idLen++;
			}
			else {
				state=-1;
				err=30;
			}
			break;

		case 31:
			c=getNextChar();
			if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')){
				state=31;
				idLen++;
			}
			else if(c <= '9' && c >= '0'){
				state=32;
				idLen++;
			}
			else
				state=33;

			if(idLen>30) {
				state=-1;
				err=31;
			}
			break;

		case 32:
			c=getNextChar();
			if(c <= '9' && c >= '0'){
				state=32;
				idLen++;
			}
			else
				state=33;

			if(idLen>30) {
				state=-1;
				err=31;
			}
			break;

		case 33:
			retract(1);
			lexeme=getLexeme();
			t.lexeme=lexeme;
			if(strcmp(lexeme,"_main") == 0) {
				t.tid=TK_MAIN;
				t.lineNo=line;
			}
			else {
				t.tid=TK_FUNID;
				t.lineNo=line;
			}
			reset();
			return t;

		case 34:
			c=getNextChar();
			if(c >= '2' && c <= '7'){
				idLen=2;
				state=35;
			}
			else if(c >= 'a' && c <= 'z')
				state=38;
			else
				state=39;
			break;

		case 35:
			c=getNextChar();
			if(c >= 'b' && c <= 'd'){
				state=35;
				idLen++;
			}
			else if(c >= '2' && c <= '7'){
				state=36;
				idLen++;
			}
			else
				state=37;
			if(idLen>20){
				state=-1;
				err=35;
			}
			break;

		case 36:
			c=getNextChar();
			if(c >= '2' && c <= '7'){
				idLen++;
				state=36;
			}
			else
				state=37;

			if(idLen>20){
				state=-1;
				err=36;
			}
			break;

		case 37:
			retract(1);
			lexeme=getLexeme();
			t.lexeme=lexeme;
			t.tid=TK_ID;
			t.lineNo=line;
			reset();
			return t;

		case 38:
			c=getNextChar();
			// fprintf(stderr, "%c\n",c );
			if(c >= 'a' && c <= 'z')
				state=38;
			else
				state=39;
			break;

		case 39:
			retract(1);
			lexeme=getLexeme();
			t.lexeme=lexeme;
			t.lineNo=line;
			t.tid=isKeyword(lexeme);
			reset();
			return t;

		case 40:
			t.tid=SENTINEL;
			return t;
		}
	}
	if(err!=-1) {
		fprintf(stdout,"\nLine:%d: LEXICAL_ERROR",line);
		lexeme=getLexeme();
		fprintf(stdout," in lexeme: %s\n",lexeme);
		free(lexeme);
		fprintf(stdout,"\tLexer state: %d\n",err);
		switch(err){
		case 0:
			fprintf(stdout, "\tUnrecognised character: %c\n",c);
			break;
		case 14:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected '='\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected '='\n",c);
			retract(1);
			break;
		case 15:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected '&'\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected '&'\n",c);
			retract(1);
			break;
		case 16:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected '&'\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected '&'\n",c);
			retract(1);
			break;
		case 17:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected '@'\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected '@'\n",c);
			retract(1);
			break;
		case 18:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected '@'\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected '@'\n",c);
			retract(1);
			break;
		case 19:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected '='\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected '='\n",c);
			retract(1);
			break;
		case 21:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected '-'\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected '-'\n",c);
			retract(1);
			break;
		case 22:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected '-'\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected '-'\n",c);
			retract(1);
			break;
		case 25:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected lower case alphabet [a-z]\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected lower case alphabet [a-z]\n",c);
			retract(1);
			break;
		case 28:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected digit [0-9]\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected digit [0-9]\n",c);
			retract(1);
			break;
		case 29:
			if(c==EOF)
				fprintf(stdout, "\tUnexpected character: EOF, expected digit [0-9]\n");
			else
				fprintf(stdout, "\tUnexpected character: '%c', expected digit [0-9]\n",c);
			retract(1);
			break;
		case 30:
			fprintf(stdout, "\tUnexpected character: '%c', expected alphabet [A-Z] or [a-z]\n",c);
			retract(1);
			break;
		case 31:
			fprintf(stdout, "\tFunction identifier length exceeds the limit of 30 characters\n");
			retract(1);
			break;
		case 32:
			fprintf(stdout, "\tFunction identifier length exceeds the limit of 30 characters\n");
			retract(1);
			break;
		case 35:
			fprintf(stdout, "\tVariable identifier length exceeds the limit of 20 characters\n");
			retract(1);
			break;
		case 36:
			fprintf(stdout, "\tVariable identifier length exceeds the limit of 20 characters\n");
			retract(1);
			break;
		}
		reset();
		t.tid=LEX_ERROR;
		return t;
	}
	assert(0);
	return t;
}
