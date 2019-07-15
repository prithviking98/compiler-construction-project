// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P

#include"common_defs.h"
#include <assert.h>
#include<string.h>

char * enumToStr (gterm id) {
	//returns the string using the gterm
	switch(id)
	{
		case 0: return "program";
				break;
		case 1: return "mainFunction";
				break;
		case 2: return "otherFunctions";
				break;
		case 3: return "function";
				break;
		case 4: return "input_par";
				break;
		case 5: return "output_par";
				break;
		case 6: return "parameter_list";
				break;
		case 7: return "dataType";
				break;
		case 8: return "primitiveDatatype";
				break;
		case 9: return "constructedDatatype";
				break;
		case 10: return "remaining_list";
				break;
		case 11: return "stmts";
				break;
		case 12: return "typeDefinitions";
				break;
		case 13: return "typeDefinition";
				break;
		case 14: return "fieldDefinitions";
				break;
		case 15: return "fieldDefinition";
				break;
		case 16: return "moreFields";
				break;
		case 17: return "declarations";
				break;
		case 18: return "declaration";
				break;
		case 19: return "global_or_not";
				break;
		case 20: return "otherStmts";
				break;
		case 21: return "stmt";
				break;
		case 22: return "assignmentStmt";
				break;
		case 23: return "singleOrRecId";
				break;
		case 24: return "new_24";
				break;
		case 25: return "funCallStmt";
				break;
		case 26: return "outputParameters";
				break;
		case 27: return "inputParameters";
				break;
		case 28: return "iterativeStmt";
				break;
		case 29: return "conditionalStmt";
				break;
		case 30: return "elsePart";
				break;
		case 31: return "ioStmt";
				break;
		case 32: return "allVar";
				break;
		case 33: return "allVarTemp";
				break;
		case 34: return "arithmeticExpression";
				break;
		case 35: return "expPrime";
				break;
		case 36: return "term";
				break;
		case 37: return "termPrime";
				break;
		case 38: return "factor";
				break;
		case 39: return "highPrecedenceOperators";
				break;
		case 40: return "lowPrecedenceOperators";
				break;
		case 41: return "all";
				break;
		case 42: return "temp";
				break;
		case 43: return "booleanExpression";
				break;
		case 44: return "var";
				break;
		case 45: return "logicalOp";
				break;
		case 46: return "relationalOp";
				break;
		case 47: return "returnStmt";
				break;
		case 48: return "optionalReturn";
				break;
		case 49: return "idList";
				break;
		case 50: return "more_ids";
				break;
		case 51: return "eps";
				break;
		case 52: return "TK_ASSIGNOP";
				break;
		case 53: return "TK_COMMENT";
				break;
		case 54: return "TK_FIELDID";
				break;
		case 55: return "TK_ID";
				break;
		case 56: return "TK_NUM";
				break;
		case 57: return "TK_RNUM";
				break;
		case 58: return "TK_FUNID";
				break;
		case 59: return "TK_RECORDID";
				break;
		case 60: return "TK_WITH";
				break;
		case 61: return "TK_PARAMETERS";
				break;
		case 62: return "TK_END";
				break;
		case 63: return "TK_WHILE";
				break;
		case 64: return "TK_TYPE";
				break;
		case 65: return "TK_MAIN";
				break;
		case 66: return "TK_GLOBAL";
				break;
		case 67: return "TK_PARAMETER";
				break;
		case 68: return "TK_LIST";
				break;
		case 69: return "TK_SQL";
				break;
		case 70: return "TK_SQR";
				break;
		case 71: return "TK_INPUT";
				break;
		case 72: return "TK_OUTUT";
				break;
		case 73: return "TK_INT";
				break;
		case 74: return "TK_REAL";
				break;
		case 75: return "TK_COMMA";
				break;
		case 76: return "TK_SEM";
				break;
		case 77: return "TK_COLON";
				break;
		case 78: return "TK_DOT";
				break;
		case 79: return "TK_ENDWHILE";
				break;
		case 80: return "TK_OP";
				break;
		case 81: return "TK_CL";
				break;
		case 82: return "TK_IF";
				break;
		case 83: return "TK_THEN";
				break;
		case 84: return "TK_ENDIF";
				break;
		case 85: return "TK_READ";
				break;
		case 86: return "TK_WRITE";
				break;
		case 87: return "TK_RETURN";
				break;
		case 88: return "TK_PLUS";
				break;
		case 89: return "TK_MINUS";
				break;
		case 90: return "TK_MUL";
				break;
		case 91: return "TK_DIV";
				break;
		case 92: return "TK_CALL";
				break;
		case 93: return "TK_RECORD";
				break;
		case 94: return "TK_ENDRECORD";
				break;
		case 95: return "TK_ELSE";
				break;
		case 96: return "TK_AND";
				break;
		case 97: return "TK_OR";
				break;
		case 98: return "TK_NOT";
				break;
		case 99: return "TK_LT";
				break;
		case 100: return "TK_LE";
				break;
		case 101: return "TK_EQ";
				break;
		case 102: return "TK_GT";
				break;
		case 103: return "TK_GE";
				break;
		case 104: return "TK_NE";
				break;
		case 105: return "SENTINEL";
				break;
		case 106: return "LEX_ERROR";
				break;
		default:
			assert(0);
			return "Something Wrong";
	}
}

gterm getTerm(char *s) {
	//returns enum based on string
	if(strcmp(s,"program")==0)
		return program;
	if(strcmp(s,"mainFunction")==0)
		return mainFunction;
	if(strcmp(s,"otherFunctions")==0)
		return otherFunctions;
	if(strcmp(s,"function")==0)
		return function;
	if(strcmp(s,"input_par")==0)
		return input_par;
	if(strcmp(s,"output_par")==0)
		return output_par;
	if(strcmp(s,"parameter_list")==0)
		return parameter_list;
	if(strcmp(s,"dataType")==0)
		return dataType;
	if(strcmp(s,"primitiveDatatype")==0)
		return primitiveDatatype;
	if(strcmp(s,"constructedDatatype")==0)
		return constructedDatatype;
	if(strcmp(s,"remaining_list")==0)
		return remaining_list;
	if(strcmp(s,"stmts")==0)
		return stmts;
	if(strcmp(s,"typeDefinitions")==0)
		return typeDefinitions;
	if(strcmp(s,"typeDefinition")==0)
		return typeDefinition;
	if(strcmp(s,"fieldDefinitions")==0)
		return fieldDefinitions;
	if(strcmp(s,"fieldDefinition")==0)
		return fieldDefinition;
	if(strcmp(s,"moreFields")==0)
		return moreFields;
	if(strcmp(s,"declarations")==0)
		return declarations;
	if(strcmp(s,"declaration")==0)
		return declaration;
	if(strcmp(s,"global_or_not")==0)
		return global_or_not;
	if(strcmp(s,"otherStmts")==0)
		return otherStmts;
	if(strcmp(s,"stmt")==0)
		return stmt;
	if(strcmp(s,"assignmentStmt")==0)
		return assignmentStmt;
	if(strcmp(s,"singleOrRecId")==0)
		return singleOrRecId;
	if(strcmp(s,"new_24")==0)
		return new_24;
	if(strcmp(s,"funCallStmt")==0)
		return funCallStmt;
	if(strcmp(s,"outputParameters")==0)
		return outputParameters;
	if(strcmp(s,"inputParameters")==0)
		return inputParameters;
	if(strcmp(s,"iterativeStmt")==0)
		return iterativeStmt;
	if(strcmp(s,"conditionalStmt")==0)
		return conditionalStmt;
	if(strcmp(s,"elsePart")==0)
		return elsePart;
	if(strcmp(s,"ioStmt")==0)
		return ioStmt;
	if(strcmp(s,"allVar")==0)
		return allVar;
	if(strcmp(s,"allVarTemp")==0)
		return allVarTemp;
	if(strcmp(s,"arithmeticExpression")==0)
		return arithmeticExpression;
	if(strcmp(s,"expPrime")==0)
		return expPrime;
	if(strcmp(s,"term")==0)
		return term;
	if(strcmp(s,"termPrime")==0)
		return termPrime;
	if(strcmp(s,"factor")==0)
		return factor;
	if(strcmp(s,"highPrecedenceOperators")==0)
		return highPrecedenceOperators;
	if(strcmp(s,"lowPrecedenceOperators")==0)
		return lowPrecedenceOperators;
	if(strcmp(s,"all")==0)
		return all;
	if(strcmp(s,"temp")==0)
		return temp;
	if(strcmp(s,"booleanExpression")==0)
		return booleanExpression;
	if(strcmp(s,"var")==0)
		return var;
	if(strcmp(s,"logicalOp")==0)
		return logicalOp;
	if(strcmp(s,"relationalOp")==0)
		return relationalOp;
	if(strcmp(s,"returnStmt")==0)
		return returnStmt;
	if(strcmp(s,"optionalReturn")==0)
		return optionalReturn;
	if(strcmp(s,"idList")==0)
		return idList;
	if(strcmp(s,"more_ids")==0)
		return more_ids;
	if(strcmp(s,"eps")==0)
		return eps;
	if(strcmp(s,"TK_ASSIGNOP")==0)
		return TK_ASSIGNOP;
	if(strcmp(s,"TK_COMMENT")==0)
		return TK_COMMENT;
	if(strcmp(s,"TK_FIELDID")==0)
		return TK_FIELDID;
	if(strcmp(s,"TK_ID")==0)
		return TK_ID;
	if(strcmp(s,"TK_NUM")==0)
		return TK_NUM;
	if(strcmp(s,"TK_RNUM")==0)
		return TK_RNUM;
	if(strcmp(s,"TK_FUNID")==0)
		return TK_FUNID;
	if(strcmp(s,"TK_RECORDID")==0)
		return TK_RECORDID;
	if(strcmp(s,"TK_WITH")==0)
		return TK_WITH;
	if(strcmp(s,"TK_PARAMETERS")==0)
		return TK_PARAMETERS;
	if(strcmp(s,"TK_END")==0)
		return TK_END;
	if(strcmp(s,"TK_WHILE")==0)
		return TK_WHILE;
	if(strcmp(s,"TK_TYPE")==0)
		return TK_TYPE;
	if(strcmp(s,"TK_MAIN")==0)
		return TK_MAIN;
	if(strcmp(s,"TK_GLOBAL")==0)
		return TK_GLOBAL;
	if(strcmp(s,"TK_PARAMETER")==0)
		return TK_PARAMETER;
	if(strcmp(s,"TK_LIST")==0)
		return TK_LIST;
	if(strcmp(s,"TK_SQL")==0)
		return TK_SQL;
	if(strcmp(s,"TK_SQR")==0)
		return TK_SQR;
	if(strcmp(s,"TK_INPUT")==0)
		return TK_INPUT;
	if(strcmp(s,"TK_OUTPUT")==0)
		return TK_OUTPUT;
	if(strcmp(s,"TK_INT")==0)
		return TK_INT;
	if(strcmp(s,"TK_REAL")==0)
		return TK_REAL;
	if(strcmp(s,"TK_COMMA")==0)
		return TK_COMMA;
	if(strcmp(s,"TK_SEM")==0)
		return TK_SEM;
	if(strcmp(s,"TK_COLON")==0)
		return TK_COLON;
	if(strcmp(s,"TK_DOT")==0)
		return TK_DOT;
	if(strcmp(s,"TK_ENDWHILE")==0)
		return TK_ENDWHILE;
	if(strcmp(s,"TK_OP")==0)
		return TK_OP;
	if(strcmp(s,"TK_CL")==0)
		return TK_CL;
	if(strcmp(s,"TK_IF")==0)
		return TK_IF;
	if(strcmp(s,"TK_THEN")==0)
		return TK_THEN;
	if(strcmp(s,"TK_ENDIF")==0)
		return TK_ENDIF;
	if(strcmp(s,"TK_READ")==0)
		return TK_READ;
	if(strcmp(s,"TK_WRITE")==0)
		return TK_WRITE;
	if(strcmp(s,"TK_RETURN")==0)
		return TK_RETURN;
	if(strcmp(s,"TK_PLUS")==0)
		return TK_PLUS;
	if(strcmp(s,"TK_MINUS")==0)
		return TK_MINUS;
	if(strcmp(s,"TK_MUL")==0)
		return TK_MUL;
	if(strcmp(s,"TK_DIV")==0)
		return TK_DIV;
	if(strcmp(s,"TK_CALL")==0)
		return TK_CALL;
	if(strcmp(s,"TK_RECORD")==0)
		return TK_RECORD;
	if(strcmp(s,"TK_ENDRECORD")==0)
		return TK_ENDRECORD;
	if(strcmp(s,"TK_ELSE")==0)
		return TK_ELSE;
	if(strcmp(s,"TK_AND")==0)
		return TK_AND;
	if(strcmp(s,"TK_OR")==0)
		return TK_OR;
	if(strcmp(s,"TK_NOT")==0)
		return TK_NOT;
	if(strcmp(s,"TK_LT")==0)
		return TK_LT;
	if(strcmp(s,"TK_LE")==0)
		return TK_LE;
	if(strcmp(s,"TK_EQ")==0)
		return TK_EQ;
	if(strcmp(s,"TK_GT")==0)
		return TK_GT;
	if(strcmp(s,"TK_GE")==0)
		return TK_GE;
	if(strcmp(s,"TK_NE")==0)
		return TK_NE;
	if(strcmp(s,"SENTINEL")==0)
		return SENTINEL;
	if(strcmp(s,"LEX_ERROR")==0)
		return LEX_ERROR;
	assert(0);
	return -1; // An error has occurred
}
