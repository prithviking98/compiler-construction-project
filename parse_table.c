// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common_defs.h"
#include "parser_preprocess.h"

typedef long long ll;

int r = eps;
int c = SENTINEL - eps;
int parseTable[eps][SENTINEL - eps];
/*rows represent non terminals, indexed by corresponding gterm
columns are terminals and EOF (SENTINEL). indexed by gterm-eps.
first column=0 corresponds to SENTINEL
sync, skip entries have values defined above
*/

// extern void printBitMask(ll);
// extern void pintNonT(int);

void initParseTable(){
	// parseTable=(int**)malloc(sizeof(int)*r*c);
	for(int i=0; i<r; ++i)
		for(int j=0; j<c; ++j)
			parseTable[i][j]=SKIP;
}

void calcParseTable(grammar g){
	//outputs parse table matrix to a file "parse_table_binary"
	//g is grammar with first and follows already calculated

	initParseTable();
	bool flag; // Whether eps can be generated from this non-term
	ll first, follow;
	for(int i = 0; i < g.numLHS; ++i){
		flag = false;
		follow=g.follow[i];
		for(int j=0; j<g.rules[i].numRHS; ++j){
			//calculate first of g.rules[i].rhs[j]
			//put parseTable[i][t]=j for all terms t in the first
			//if eps in above first set
				//then put j in all t in follow of lhs, flag=false
			first=calcFirstSetSentential(g,i,j,0);
			if(first & MASK_eps){
				flag = true;
				for(int k=0; k<c; ++k){
					if((follow & (1LL<<k)) != 0)
						parseTable[i][k]=j;
				}
			}
			for(int k=1; k<c; ++k){ // k = 1 since epsilon shouldn't be included
				if(first & (1LL << k))
					parseTable[i][k]=j;
			}
		}
		if(!flag) {
			for(int k=0; k<c; ++k){
				if((follow & (1LL<<k)) != 0 && parseTable[i][k] == SKIP) {
					parseTable[i][k]=SYNC;
				}
			}
		}
	}
	/*
	FILE *fptr =fopen("temp.txt","w");
	for(int i=0; i<r; ++i){
		for(int j=0;j<c;++j)
			fprintf(fptr, "%d ", parseTable[i][j]);
		fprintf(fptr, "\n");
	}
	fclose(fptr);
	*/
}

