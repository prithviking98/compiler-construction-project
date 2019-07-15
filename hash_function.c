// Group 9
// Prithvi Raj   - 2016A7PS0013P
// Anuvind Bhat  - 2016A7PS0098P
// Rahul Saxena  - 2016A7PS0027P
// Siddhant Jain - 2016A7PS0113P
#include "hash_function.h"

int hash(char *str, int m) {
	int h = 5381;
	while (*str != '\0') {
		h = (h * 33) ^ *str;
		++str;
	}
	h %= m;
	if (h < 0) {
		h += m;
	}
	return h;
}
