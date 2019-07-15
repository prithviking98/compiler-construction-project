// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhanr Jain - 2016A7PS0113P

#include"parser_preprocess.h"
#include<sys/time.h>

bool testGrammar(grammar g)
{
	int i,j,k;
	for(i=0;i<g.numLHS;i++)
	{
		if(g.first[i] & MASK_eps)
		{
			//this can become epsilon
			//so its first and follow cannot have an intersection
			if((g.first[i] & ~MASK_eps) & (g.follow[i] & ~MASK_dollar))
			{
				printf("%d can become epsilon and has an intersection between first and follow\n",i+1);
				return false;
			}
		}
		for(j=0;j<g.rules[i].numRHS;j++)
		{
			for(k=j+1;k<g.rules[i].numRHS;k++)
			{
				ll first1 = calcFirstSetSentential(g,i,j,0);
				ll first2 = calcFirstSetSentential(g,i,k,0);
				if(first1 & first2)
				{
					printf("for %d nont, rules %d and %d have an intersecting First\n",i+1,j+1,k+1);
					return false;
				}
			}
		}
	}
	return true;
}

int main()
{
	struct timeval t1,t2;
  	double elapsedTime;
  	gettimeofday(&t1,NULL);
	FILE *f = fopen("grammar.txt","r");
	grammar g = readGrammar(f);
	fclose(f);
	bool compatible = testGrammar(g); //for LL1 compatibility
	gettimeofday(&t2,NULL);
	elapsedTime = (t2.tv_sec-t1.tv_sec)*1000.0;
	elapsedTime += (t2.tv_usec-t1.tv_usec)/1000.0;
	//printf("Getting the complete grammar and checking it uses %f s\n", elapsedTime);
	/*if(compatible)
		printf("Grammar is LL(1) compatible\n");
	else
		printf("Grammar is not LL(1) compatible\n");*/
	printGrammar(g);
	//printFirst(g);
	//printFollow(g);
	calcParseTable(g);
	return 0;
}
