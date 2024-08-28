// Define all helper functions for hw2 in this file
#include "helpers2.h"

int is_delim(char c, char *delim) {
	if (delim == 0) {
		return 0;
	}
	while (*delim != '\0') {
		if (c== *delim) {
			return 1;
		}
		delim++;
	}
	return 0;
}
/*
int myStrCmp(char *x, char *y) {
	while (1) {
		if (*x != *y) {
			return 0;
		}
		if (*x == '\0' && *y == '\0') {
			return 1;
		}
		if (*x == '\0' || *y == '\0') {
			return 0;
		}
		x++;
		y++;
	}
}
*/

int myStrCmp(const char *x, const char *y) {
	int f = 0;
	while (1) {
		if (*x > *y) {
			return 1;
		} else if (*x < *y) {
			return -1;
		} else if (*x == '\0' && *y == '\0') {
			return 0;
		}
		x++;
		y++;
	}
	return f;
}

unsigned int myStrLen(const char *s) {
	int i = 0;
	while (*s != '\0') {
		i++;
		s++;
	}
	return i;
}


