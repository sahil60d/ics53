// Declare all helper functions for hw1 in this file
#include "../include/helpers1.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>


int n_helper(int argc){
	if (argc > 2) {
		return 1;
	}
	char c;					//character from stdin
	int flag = 0;				// 0 if previous c was str, 1 if int
	int numflag = 0;			// 0 if no digits, 1 if there are
	unsigned char min = 0x30;		// hex for 0
	unsigned char max = 0x39;		// hex for 9

	while (!feof(stdin)) {
		c = getchar();
		if (c >= min && c <= max) {	// checks if c is between 0-9
			flag = 1;
			numflag = 1;
			printf("%c", c);
		} else {
			if (flag == 1) {
				printf("\n");
			}
		flag = 0;
		}	
	}

	if (numflag != 1) {
		return 2;
	}
	return 0;
}

int l_helper(int argc, char *argv[]) {
	int Iflag = 0;
	int Sflag = 0;

	if (argc >= 4) {
	       if (!strcmp(argv[3], "-S")) {
		       Sflag = 1;
	       } else if (!strcmp(argv[3], "-I")) {
		       Iflag = 1;
	       } else {
		       return 1;
	       }

	       if (argc == 5) {
		       if (!strcmp(argv[4], "-S") && !Sflag) {
			       Sflag = 1;
		       } else if(!strcmp(argv[4], "-I") && !Iflag) {
				Iflag = 1;
		       } else {
			       return 1;
		       }
	       }	       
	}	       
		
	char word[100];
	strcpy(word, argv[2]);
	
	if (Iflag == 1) {
		makeSmall(word);
	}

	int numcounter = 0;
	int charcount = 0;
	char str[1000];
	char delim[] = " \t\r\n";
	int line = 1;
	while(!feof(stdin)) {
		if (fgets(str, 1000, stdin) == NULL) {
			line++;
			break;
		}
		

		charcount += strlen(str);

		int numword = 1;
		char *strptr = strtok(str, delim);
		while (strptr != NULL) {
			if (Iflag == 1) {
				makeSmall(strptr);
			}

			if(!strcmp(strptr, word)) {
				printf("%d:%d\n", line, numword);
				numcounter++;
			}
			strptr = strtok(NULL, delim);
			numword++;
		}
		line++;
	}

	line--;

	if (numcounter > 0) {
		if (argc > 3 && !strcmp(argv[3], "-S")) {
			fprintf(stderr,"%d %d %d\n", numcounter, charcount, line);
		} else  {
			fprintf(stderr,"%d\n", numcounter);
		}
		return 0;
	} 

	return 2;

}

void makeSmall(char *s){
	for (int i = 0; s[i]; i++) {
		s[i] = tolower(s[i]);
	}
}



int h_helper(int argc, char *argv[]) {
	if (argc <= 2) {
		return 1;
	}

	int opt;
	int s_flag = 0;
	int i_flag = 0;
	int c_flag = 0;
	int fg = 37;
	int bg = 41;

	//set flags
	while((opt = getopt(argc, argv, ":nl:h:C::SI")) != -1) {
		switch(opt) {
			case 'S':
				s_flag = 1;
				break;
			case 'I':
				i_flag = 1;
				break;
			case 'C':
				c_flag = 1;
				//fg = atoi(argv[optind]);
				//bg = atoi(argv[optind + 1]);
				break;
			case ':':
				break;
			case '?':
				return 1;
		}
	}

	char word[100];
	strcpy(word, argv[2]);
	int wordlen = strlen(word);
	char highlight[100];
	strcpy(highlight, "");
	int highlight_count = 0;
	int clear_flag = 0;
	char c;
	int char_count = 0;
	int line_count = 0;

	//i flag
	if (i_flag) {
		makeSmall(word);
	}

	if (fg < 30 || fg > 37 || bg < 40 || bg > 47) {
		return 1;
	}

	//loop through lines
	while (1) {
		c = getchar();
		if (c == EOF) {
			break;
		}
		char_count++;
		if (i_flag) {
			c = tolower(c);
		}

		if (c == ' ' || c == '\n' || c == '\t' || c == '\r'){
			if (c == '\n') {
				line_count++;
			}
			// check if match or clear highlight
			if (!strcmp(highlight, word)){
				highlight_count++;
				// print in color 
				if (c_flag == 1) {
					//printf("\x1B[%d;%dm%s\x1B[39;49m", fg, bg, highlight);
					printf("\x1B[37;41m%s\x1B[39;49m", highlight);
				} else {
					printf("\x1B[37;41m%s\x1B[39;49m", highlight);
				}
				
			} else {
				printf("%s", highlight);
			}
			clear_flag = 1;
		}

		strcat(highlight, &c);

		if (clear_flag) {
			printf("%c", c);
			strcpy(highlight, "");
			clear_flag = 0;
		}

	}

	// print newline
	if (highlight_count == 0) {
		return 2;
	}

	//s flag
	if(s_flag) {
		fprintf(stderr, "%d, %d, %d\n", highlight_count, char_count, line_count);
	}

	return 0;
}

