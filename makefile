compileAll: ast.o code_gen.o common_defs.o hash_function.o lexer.o linked_list_id.o linked_list_rec.o linked_list_func.o new_driver.o parse_table.o parser.o parser_preprocess.o semantic_analyser.o table_id.o table_func.o table_rec.o type_extraction_id.o type_extraction_rec.o type_extraction_func.o
	gcc -Wall -Wextra -O2 -o compiler ast.o code_gen.o common_defs.o hash_function.o lexer.o linked_list_id.o linked_list_rec.o linked_list_func.o new_driver.o parse_table.o parser.o parser_preprocess.o semantic_analyser.o table_id.o table_func.o table_rec.o type_extraction_id.o type_extraction_rec.o type_extraction_func.o

ast.o: ast.c ast.h
	gcc -Wall -Wextra -O2 -c ast.c -o ast.o

hash_function.o: hash_function.c hash_function.h
	gcc -Wall -Wextra -O2 -c hash_function.c -o hash_function.o

linked_list_id.o: linked_list_id.c linked_list_id.h
	gcc -Wall -Wextra -O2 -c linked_list_id.c -o linked_list_id.o

linked_list_rec.o: linked_list_rec.c linked_list_rec.h
	gcc -Wall -Wextra -O2 -c linked_list_rec.c -o linked_list_rec.o

linked_list_func.o: linked_list_func.c linked_list_func.h
	gcc -Wall -Wextra -O2 -c linked_list_func.c -o linked_list_func.o

new_driver.o: new_driver.c
	gcc -Wall -Wextra -O2 -c new_driver.c -o new_driver.o

code_gen.o: code_gen.c code_gen.h
	gcc -Wall -Wextra -O2 -c code_gen.c -o code_gen.o

common_defs.o: common_defs.c common_defs.h
	gcc -Wall -Wextra -O2 -c common_defs.c -o common_defs.o

parser_preprocess.o: parser_preprocess.c parser_preprocess.h
	gcc -Wall -Wextra -O2 -c parser_preprocess.c -o parser_preprocess.o

linked_list.o: linked_list.c linked_list.h
	gcc -Wall -Wextra -O2 -c linked_list.c -o linked_list.o

lexer.o: lexer.c common_defs.h lexer.h
	gcc -Wall -Wextra -O2 -c lexer.c -o lexer.o

parse_table.o: parse_table.c common_defs.h parser_preprocess.h
	gcc -Wall -Wextra -O2 -c parse_table.c -o parse_table.o

parser.o: parser.c common_defs.h lexer.h parser.h
	gcc -Wall -Wextra -O2 -c parser.c -o parser.o

semantic_analyser.o: semantic_analyser.c semantic_analyser.h
	gcc -Wall -Wextra -O2 -c semantic_analyser.c -o semantic_analyser.o

table_func.o: table_func.c table_func.h
	gcc -Wall -Wextra -O2 -c table_func.c -o table_func.o

table_rec.o: table_rec.c table_rec.h
	gcc -Wall -Wextra -O2 -c table_rec.c -o table_rec.o

table_id.o: table_id.c table_id.h
	gcc -Wall -Wextra -O2 -c table_id.c -o table_id.o

type_extraction_func.o: type_extraction_func.c type_extraction.h
	gcc -Wall -Wextra -O2 -c type_extraction_func.c -o type_extraction_func.o

type_extraction_id.o: type_extraction_id.c type_extraction.h
	gcc -Wall -Wextra -O2 -c type_extraction_id.c -o type_extraction_id.o

type_extraction_rec.o: type_extraction_rec.c type_extraction.h
	gcc -Wall -Wextra -O2 -c type_extraction_rec.c -o type_extraction_rec.o



.PHONY: clean
clean:
	rm -f *.o compiler
