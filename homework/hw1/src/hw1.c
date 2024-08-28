#include "hw1.h"
#include <stdio.h>
#include <string.h>

//Main
int main(int argc, char *argv[]) {
	//insert code
	if (argc <= 1) {
		fprintf(stderr, USAGE); 
		return 1;
	}

	int exit_status = 0;
		
	if (!strcmp(argv[1], "-n")) {
		if (argc > 3) {
			fprintf(stderr, USAGE);
			return 1;
		}
		exit_status = n_helper(argc);
	} else if (!strcmp(argv[1], "-l")) {
		if (argc > 5) {
			fprintf(stderr,USAGE);
			return 1;
		}
		exit_status = l_helper(argc, argv);
	} else if (!strcmp(argv[1], "-h")) {
		if (argc > 8) {
			fprintf(stderr,USAGE);
			return 1;
		}
		exit_status =  h_helper(argc, argv);
	}
	else {
		fprintf(stderr, USAGE);
		return 1;
	}
	
	if (exit_status == 1) {
		fprintf(stderr, USAGE);
	}

	return exit_status;
}
