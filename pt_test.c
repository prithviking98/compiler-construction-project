// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#include "parser.h"
#include "common_defs.h"
#include "parser_preprocess.h"
#include <assert.h>

int ptRows = eps;
int ptCols = SENTINEL - eps;
int pt[eps][SENTINEL-eps];

extern int parseTable[eps][SENTINEL-eps];
void readParseTable() {
	//gets the parse table stored in parse_table_binary
	FILE *fptr = fopen("parse_table_binary", "rb");
	assert(fptr != NULL);
	fread(pt, sizeof(int), ptRows * ptCols, fptr);
	fclose(fptr);
}

int main(){
	grammar g=readGrammar(fopen("grammar.txt","r"));
	calcParseTable(g);
	readParseTable();
	fprintf(stderr, "read PT\n" );
	for(int i=0; i<ptRows; ++i){
		for(int j=0;j<ptCols;++j)
			fprintf(stderr, "%d", pt[i][j]==parseTable[i][j]);
		fprintf(stderr, "\n");
	}

}
