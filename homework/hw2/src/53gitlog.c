#include "constants2.h"
#include "helpers2.h"
#include "hw2.h"
#include <time.h>

int main(int argc, char* argv[]) {
	int D_flag = 0;
	int A_flag = 0;
    int NUM_arg = -1;
    int LEVEL_arg = 0;
    char ORDER_arg = 'f';   // default no order
    char* DATE_arg = NULL;
    char* OUTFILE = NULL;
    char* INFILE = NULL;

    // Use basic getopt to parse flags and respective arguments
    int option;
    while ((option = getopt(argc, argv, "HD:A:O:I:n:ad" )) >= 0) {
        switch (option) {
            case 'H':
				fprintf(stdout, USAGE_MSG);
				return EXIT_SUCCESS;
            case 'D':
        	    D_flag = 1;
                DATE_arg = optarg;
                break;
            case 'A':
				A_flag = 1;
                LEVEL_arg = atoi(optarg);
                break;
            case 'n':
				NUM_arg = atoi(optarg);
                break;
            case 'a':
				ORDER_arg = option;
                break;
            case 'O':
				OUTFILE = optarg;
                break;
            case 'I':
				INFILE = optarg;
                break;
            default:
                fprintf(stderr, USAGE_MSG);
                return EXIT_FAILURE;
        }
    }

    // validate a required option was specified - Does not check for more than 1
    if ( ! (A_flag | D_flag) )
    {
        fprintf(stderr, "ERROR: Search mode was not specified.\n\n" USAGE_MSG);
        return EXIT_FAILURE;
    }
    
    // INSERT YOUR IMPLEMENTATION HERE
    // getopts only stored the arguments and performed basic checks. More error checking is still needed!!!!
	FILE *fileIN = stdin;
	FILE *fileOUT = stdout;
    char* line = (char*)malloc(500 * sizeof(char));
	long int *ts;
	node_t *search;

	list_t *list = CreateList(&AuthorEmailComparator, &AuthorPrinter, &AuthorDeleter);

	if (OUTFILE != NULL) {
		fileOUT = fopen(OUTFILE, "w");
		if (fileOUT == NULL) {
			fprintf(stderr, USAGE_MSG);
			return EXIT_FAILURE;
		}
	} else if (INFILE != NULL) {
		fileIN = fopen(INFILE, "r");
		if (fileIN == NULL) {
			fprintf(stderr, USAGE_MSG);
			return EXIT_FAILURE;
		}
	}

	while (!feof(fileIN)) {
		fgets(line, 500, fileIN);
		Author *a = malloc(sizeof(Author)); 
		a = CreateAuthor(line, ts);
		search = FindInList(list, a);
		if (search != NULL) {
			ProcessModFile(fileIN, ((Author *)(search->data))->modFileList, ORDER_arg);
			list->deleter(a);
			free(a);
		} else {
			ProcessModFile(fileIN, a->modFileList, ORDER_arg);
			if (ORDER_arg == 'f') {
				InsertAtTail(list, a);
			} else if (ORDER_arg == 'a') {
				InsertInOrder(list, a);
			}
		}
		fgets(line, 500, fileIN);
	}
	free(line);

	if (A_flag == 1) {
		node_t *n = list->head;
		if (n == 0) {
			return 0;
		}
		Author *a = n->data;

		if (NUM_arg == 0) {
			return 0;
		} else if (NUM_arg != -1) {
			for (int i = 0; i < NUM_arg; i++) {
				AuthorPrinter(a, fileOUT, LEVEL_arg);
				n = n->next;
				if (n==0) {
					return 0;
				}
				a = n->data;
			}
		} else {
			while(1) {
				AuthorPrinter(a, fileOUT, LEVEL_arg);
				n = n->next;
				if (n==0) { 
					return 0;
				}
				a = n->data;
			}
		}
	} else if (D_flag == 1) {
		fprintf(stdout, "hi");
	}
    return 0;
}
