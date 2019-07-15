// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#include "parser_preprocess.h"
#include <string.h>
#include <stdlib.h>

char *getLHS(char *s, int len) {
	//gets the LHS of a grammar rule i.e. s
	//the string in the first <> is the LHS
	//used when generating grammar
	int i,j,k;
	i = 0;
	while(i<len&&s[i]!='<')
		i++;
	j = i+1;
	i = j;
	while(i<len&&s[i]!='>')
		i++;
	int ret_len = i-j+1;
	char *lhs = malloc(sizeof(char)*ret_len);
	for(k=0;k<ret_len-1;k++)
		lhs[k] = s[j+k];
	lhs[k] = '\0';
	return lhs;
}

char *getAllRHS(char *s, int len) {
	//gets all the RHS's possible from a given grammar rule
	//everything after the ===> is the RHS
	//used when generating the grammar
	int i,j;
	for(i=0;i<len;i++)
		if(s[i]=='=')
			break;
	//s[i],s[i+1],s[i+2] is '=' and s[i+3] is '>'
	i += 4;
	int ret_len = len-i+1;
	char *rhs = malloc(sizeof(char)*ret_len);
	for(j=0;j<ret_len-1;j++)
		rhs[j] = s[j+i];
	rhs[j] = '\0';
	return rhs;
}

char *getOneRHS(char *rhs, int *pos, int len) {
	//from a given RHS, starting from one position (pos)
	//it gets exactly 1 RHS (i.e. terminated by | or \n)
	//updates pos in the process
	//used when generating grammar
	int i = *pos;
	while(i<len)
	{
		if(rhs[i]=='|')
		{
			i--;
			while(rhs[i]==' ')
				i--;
			break;
		}
		else
			i++;
	}
	if(i==len)
		i--;
	//from *pos....i we have 1 RHS
	//*pos should now be made i+1
	int ret_len = i-*pos+2;
	int j = 0;
	char *one_rhs = malloc(sizeof(char)*ret_len);
	for(j=0;j<ret_len-1;j++)
		one_rhs[j] = rhs[*pos+j];
	one_rhs[j] = '\0';
	*pos = i+1;
	return one_rhs;
}

char *get_NonTerm(char *one_rhs, int *pos, int len) {
	//from 1 RHS, starting from the position (pos)
	//it gets one non terminal (between <>)
	//updtaes pos and used when generating grammar
	//one_rhs[pos]=='<'
	int i = *pos;
	while(i<len)
		if(one_rhs[i]=='>')
			break;
		else
			i++;
	//*pos+1...i-1 is the non_term
	//*pos should now be made i+1
	int ret_len = i-*pos;
	char *non_term = malloc(sizeof(char)*ret_len);
	int j;
	for(j=0;j<ret_len-1;j++)
		non_term[j] = one_rhs[j+*pos+1];
	non_term[j] = '\0';
	*pos = i+1;
	return non_term;
}

char *get_Term(char *one_rhs, int *pos, int len) {
	//from 1 RHS, starting from the position (pos)
	//it gets one terminal (between <>)
	//updtaes pos and used when generating grammar
	//one_rhs[pos]=='T'
	int i = *pos;
	while(i<len)
		if(one_rhs[i]==' '||one_rhs[i]=='<'||one_rhs[i]=='e')
			break;
		else
			i++;
	//term from *pos...i-1
	//*pos should now be i
	int ret_len = i-*pos+1;
	char *term = malloc(sizeof(char)*ret_len);
	int j;
	for(j=0;j<ret_len-1;j++)
		term[j] = one_rhs[j+*pos];
	term[j] = '\0';
	*pos = i;
	return term;
}

void calcFirstSet(grammar g, bool *done, int nont, ll *first_sets) {
	//using the given grammar g for the given non terminal (non t)
	//and precalculated first_sets and done (which stores which first sets have been computed completely)
	//it updates first_sets and done
	if (done[nont]) {
		return;
	}
	int i, j;
	for (i = 0; i < g.rules[nont].numRHS; i++) {
		for (j = 0; j < g.rules[nont].sizeRHS[i]; j++) {
			if (g.rules[nont].rhs[i][j].isTerminal) {
				int temp = g.rules[nont].rhs[i][j].name - eps;
				first_sets[nont] |= (1LL << temp);
				break;
			}
			else {
				calcFirstSet(g, done, g.rules[nont].rhs[i][j].name, first_sets);
				if ((first_sets[g.rules[nont].rhs[i][j].name] & MASK_eps) == 0) {
					//epsilon is NOT part of first of this non terminal
					first_sets[nont] |= first_sets[g.rules[nont].rhs[i][j].name];
					break;
				}
				else {
					if (j != g.rules[nont].sizeRHS[i] - 1)
						first_sets[nont] |= first_sets[g.rules[nont].rhs[i][j].name] & ~MASK_eps;
					else
						first_sets[nont] |= first_sets[g.rules[nont].rhs[i][j].name];
				}
			}
		}
	}
	done[nont] = true;
}

ll * first(grammar g) {
	//return first set for g
	ll *first_sets = calloc(g.numLHS, sizeof(ll));
	int i;
	bool *done = malloc(sizeof(bool) * g.numLHS);
	for(i = 0; i < g.numLHS; i++) {
		done[i] = false;
	}
	// Assumes non terminals are numbered from 0
	for(i = 0; i < g.numLHS; i++) {
		calcFirstSet(g, done, i, first_sets);
	}
	free(done);
	return first_sets;
}

ll calcFirstSetSentential(grammar g, int lhsNum, int rhsNum, int startNum) {
	//for the given grammar, this calculates the first set of the sentential form
	//in rule beginning with lhsNum, rhs being rhsNum, starting at startNum
	int i;
	ll firstSet = 0LL;
	for (i = startNum; i < g.rules[lhsNum].sizeRHS[rhsNum]; ++i) {
		if (g.rules[lhsNum].rhs[rhsNum][i].isTerminal) {
			firstSet |= 1LL << (g.rules[lhsNum].rhs[rhsNum][i].name - eps);
			break;
		}
		ll temp = g.first[g.rules[lhsNum].rhs[rhsNum][i].name];
		if ((temp & MASK_eps) == 0) {
			firstSet |= temp;
			break;
		}
		temp = (i == g.rules[lhsNum].sizeRHS[rhsNum] - 1) ? temp : (temp & ~MASK_eps);
		firstSet |= temp;
	}
	return firstSet;
}

void calcFollowSet(grammar g, bool *done, int nont, ll *follow_sets, ll deps) {
	//using grammar g, the boolean done vector (which stores which all non terminals are done with follow set calc)
	//it calculates the follow for nont and updates follow_sets
	//deps is a bitmask used to represent which all non-terminals depend on nont in follow calculation
	//and if it cycles to itself then ignore
	if (done[nont]) {
		return;
	}
	int i, j, k;
	for (i = 0; i < g.numLHS; ++i) {
		for (j = 0; j < g.rules[i].numRHS; ++j) {
			for (k = 0; k < g.rules[i].sizeRHS[j]; ++k) {
				if (g.rules[i].rhs[j][k].name == nont) {
					if (k == g.rules[i].sizeRHS[j] - 1) {
						if (((1LL << g.rules[i].lhs.name) & deps) == 0) {
							ll temp = deps | (1LL << g.rules[i].lhs.name);
							calcFollowSet(g, done, g.rules[i].lhs.name, follow_sets, temp);
						}
						follow_sets[nont] |= follow_sets[g.rules[i].lhs.name];
					}
					else {
						ll temp = calcFirstSetSentential(g, i, j, k + 1);
						follow_sets[nont] |= temp & ~MASK_eps;
						if ((temp & MASK_eps) != 0) {
							if (((1LL << g.rules[i].lhs.name) & deps) == 0) {
								ll temp = deps | (1LL << g.rules[i].lhs.name);
								calcFollowSet(g, done, g.rules[i].lhs.name, follow_sets, temp);
							}
							follow_sets[nont] |= follow_sets[g.rules[i].lhs.name];
						}
					}
				}
			}
		}
	}
	done[nont] = true;
}

ll * follow(grammar g) {
	//calculates the follow set for grammar g's nonterminals
	ll *follow_sets = calloc(g.numLHS, sizeof(ll));
	follow_sets[program] |= MASK_dollar;
	int i;
	bool *done = malloc(sizeof(bool) * g.numLHS);
	for (i = 0; i < g.numLHS; ++i) {
		done[i] = false;
	}
	for (i = 0; i < g.numLHS; ++i) {
		calcFollowSet(g, done, i, follow_sets, 1LL << i);
	}
	free(done);
	return follow_sets;
}

grammar readGrammar(FILE *fptr) {
	//returns grammar with the rules in numeric form and first and follow sets
	grammar g;
	g.numLHS = 0;
	g.rules = NULL;
	char *s = malloc(sizeof(char)*200);
	while(EOF!=fscanf(fptr,"%*d%*c%*c%[^\n]\n",s))
	{
		int sLen = strlen(s);
		char *lhs = getLHS(s, sLen);
		gterm lhs_term = getTerm(lhs);
		free(lhs);
		g.numLHS++;
		g.rules = realloc(g.rules,sizeof(rule)*g.numLHS);
		g.rules[g.numLHS-1].lhs.isTerminal = false;
		g.rules[g.numLHS-1].lhs.name = lhs_term;
		g.rules[g.numLHS-1].rhs = NULL;
		g.rules[g.numLHS-1].numRHS = 0;
		g.rules[g.numLHS-1].sizeRHS = NULL;
		char *rhs = getAllRHS(s, sLen);
		int i = 0;
		int rhsLen = strlen(rhs);
		while(i<rhsLen)
		{
			while(i<rhsLen&&((rhs[i]==' ')||(rhs[i]=='|')))
				i++;
			if(i==rhsLen)
				break;
			char *one_RHS = getOneRHS(rhs,&i, rhsLen); //got one more RHS
			g.rules[g.numLHS-1].numRHS++; //increasing numRHS
			g.rules[g.numLHS-1].sizeRHS = realloc(g.rules[g.numLHS-1].sizeRHS,sizeof(int)*g.rules[g.numLHS-1].numRHS); //need to know size of this rule
			g.rules[g.numLHS-1].sizeRHS[g.rules[g.numLHS-1].numRHS-1] = 0; //currently setting the size of this RHS to 0
			g.rules[g.numLHS-1].rhs = realloc(g.rules[g.numLHS-1].rhs,sizeof(gSymbol*)*g.rules[g.numLHS-1].numRHS); //the actual array of terms in the rule
			g.rules[g.numLHS-1].rhs[g.rules[g.numLHS-1].numRHS-1] = NULL; //setting it to NULL right now
			int j = 0;
			int one_RHSLen = strlen(one_RHS);
			while(j<one_RHSLen)
			{
				if(one_RHS[j]==' ')
				{
					j++;
					continue;
				}
				gterm add_term;
				g.rules[g.numLHS-1].sizeRHS[g.rules[g.numLHS-1].numRHS-1]++; //size of this RHS increases
				g.rules[g.numLHS-1].rhs[g.rules[g.numLHS-1].numRHS-1] = realloc(g.rules[g.numLHS-1].rhs[g.rules[g.numLHS-1].numRHS-1],sizeof(gSymbol)*g.rules[g.numLHS-1].sizeRHS[g.rules[g.numLHS-1].numRHS-1]); //space for this RHS term
				if(one_RHS[j]=='<')
				{
					char *non_term  = get_NonTerm(one_RHS,&j, one_RHSLen);
					add_term = getTerm(non_term);
					free(non_term);
					g.rules[g.numLHS-1].rhs[g.rules[g.numLHS-1].numRHS-1][g.rules[g.numLHS-1].sizeRHS[g.rules[g.numLHS-1].numRHS-1]-1].isTerminal = false; //this is a non term
				}
				else if(one_RHS[j]=='T')
				{
					char *term = get_Term(one_RHS,&j, one_RHSLen);
					add_term = getTerm(term);
					free(term);
					g.rules[g.numLHS-1].rhs[g.rules[g.numLHS-1].numRHS-1][g.rules[g.numLHS-1].sizeRHS[g.rules[g.numLHS-1].numRHS-1]-1].isTerminal = true; //this is a term
				}
				else
				{
					//its eps
					add_term = eps;
					j += 3;
					g.rules[g.numLHS-1].rhs[g.rules[g.numLHS-1].numRHS-1][g.rules[g.numLHS-1].sizeRHS[g.rules[g.numLHS-1].numRHS-1]-1].isTerminal = true; //this is a term
				}
				g.rules[g.numLHS-1].rhs[g.rules[g.numLHS-1].numRHS-1][g.rules[g.numLHS-1].sizeRHS[g.rules[g.numLHS-1].numRHS-1]-1].name = add_term; //adding the apt term
			}
			free(one_RHS);
		}
		free(rhs);
	}
	free(s);
	g.first = first(g);
	g.follow = follow(g);
	return g;
}

void printRule(rule r) {
	//prints the rule in the following format
	//LHS: 0 (non_terminal) TOKEN_NUM RHS: (rules seperated by |) where every rule is of the form (0/1(nont/t) TOKEN_NUM)
	printf("LHS: ");
	//printf("%d %s ", r.lhs.isTerminal, enumToStr(r.lhs.name));
	printf("%d ", r.lhs.name);
	printf("RHS: ");
	int i,j;
	for(i=0;i<r.numRHS;i++)
	{
		//printing RHS stored in r.rhs[i]
		//this length is r.sizeRHS[i]
		for(j=0;j<r.sizeRHS[i];j++)
			//printf("%d %s ", r.rhs[i][j].isTerminal, enumToStr(r.rhs[i][j].name));
			printf("%d ", r.rhs[i][j].name);
		if(i!=r.numRHS-1)
			printf("| ");
	}
}

void printGrammar(grammar g) {
	//prints grammar
	int i;
	for(i=0;i<g.numLHS;i++)
	{
		printRule(g.rules[i]);
		printf("\n");
	}
}

void printNonT(int pos) {
	//prints the apt TOKEN name based on its position in the bit mask
	switch(pos)
	{
		case 0: printf("eps/SENTINEL ");
				break;
		case 1: printf("TK_ASSIGNOP ");
				break;
		case 2: printf("TK_COMMENT ");
				break;
		case 3: printf("TK_FIELDID ");
				break;
		case 4: printf("TK_ID ");
				break;
		case 5: printf("TK_NUM ");
				break;
		case 6: printf("TK_RNUM ");
				break;
		case 7: printf("TK_FUNID ");
				break;
		case 8: printf("TK_RECORDID ");
				break;
		case 9: printf("TK_WITH ");
				break;
		case 10: printf("TK_PARAMETERS ");
				break;
		case 11: printf("TK_END ");
				break;
		case 12: printf("TK_WHILE ");
				break;
		case 13: printf("TK_TYPE ");
				break;
		case 14: printf("TK_MAIN ");
				break;
		case 15: printf("TK_GLOBAL ");
				break;
		case 16: printf("TK_PARAMETER ");
				break;
		case 17: printf("TK_LIST ");
				break;
		case 18: printf("TK_SQL ");
				break;
		case 19: printf("TK_SQR ");
				break;
		case 20: printf("TK_INPUT ");
				break;
		case 21: printf("TK_OUTUT ");
				break;
		case 22: printf("TK_INT ");
				break;
		case 23: printf("TK_REAL ");
				break;
		case 24: printf("TK_COMMA ");
				break;
		case 25: printf("TK_SEM ");
				break;
		case 26: printf("TK_COLON ");
				break;
		case 27: printf("TK_DOT ");
				break;
		case 28: printf("TK_ENDWHILE ");
				break;
		case 29: printf("TK_OP ");
				break;
		case 30: printf("TK_CL ");
				break;
		case 31: printf("TK_IF ");
				break;
		case 32: printf("TK_THEN ");
				break;
		case 33: printf("TK_ENDIF ");
				break;
		case 34: printf("TK_READ ");
				break;
		case 35: printf("TK_WRITE ");
				break;
		case 36: printf("TK_RETURN ");
				break;
		case 37: printf("TK_PLUS ");
				break;
		case 38: printf("TK_MINUS ");
				break;
		case 39: printf("TK_MUL ");
				break;
		case 40: printf("TK_DIV ");
				break;
		case 41: printf("TK_CALL ");
				break;
		case 42: printf("TK_RECORD ");
				break;
		case 43: printf("TK_ENDRECORD ");
				break;
		case 44: printf("TK_ELSE ");
				break;
		case 45: printf("TK_AND ");
				break;
		case 46: printf("TK_OR ");
				break;
		case 47: printf("TK_NOT ");
				break;
		case 48: printf("TK_LT ");
				break;
		case 49: printf("TK_LE ");
				break;
		case 50: printf("TK_EQ ");
				break;
		case 51: printf("TK_GT ");
				break;
		case 52: printf("TK_GE ");
				break;
		case 53: printf("TK_NE ");
				break;
		case 54: printf("TK_EQ ");
				break;
	}
}

void printBitMask(ll mask) {
	//used to print bit mask in the string form
	int i,bit;
	for(i=0;i<54;i++)
	{
		bit = mask%2;
		mask = mask/2;
		if(bit)
			printNonT(i);
	}
}

void printFirst(grammar g)
{
	//prints First sets of the grammar in string form
	int i;
	for(i=0;i<g.numLHS;i++)
	{
		printf("%d. first: ",i+1);
		printBitMask(g.first[i]);
		printf("\n");
	}
}

void printFollow(grammar g)
{
	//prints First sets of the grammar in string form
	int i;
	for(i=0;i<g.numLHS;i++)
	{
		printf("%d. follow: ",i+1);
		printBitMask(g.follow[i]);
		printf("\n");
	}
}
