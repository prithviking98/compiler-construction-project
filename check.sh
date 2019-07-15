#!/bin/bash
gcc -Wall -Wextra -O0 -g parser_preprocess.c parse_table.c check_driver.c common_defs.c -o check.out
