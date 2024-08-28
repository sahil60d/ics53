// Sahil Desai
// sjdesai

#include "../include/hw2.h"
#include "../include/helpers2.h"
#include "../include/linkedlist.h"

// Part 0 Function to implement
char* myStrCpy(char* str, char* delimiters) {
	
	if (str == 0) {
		return NULL;
	}

	int size;
	char *s = str;

	for (size = 0; *(s + size) != '\0'; ++size) {

	}
	
    char *r = (char*)malloc(size * sizeof(char) + 1);
	if (r == NULL) {
		return NULL;
	}
	char *return_str = r;

	while (1) {
		if (is_delim(*str, delimiters)) {
			*return_str = '\0';
			break;
		} else if (*str == '\0') {
			*return_str = '\0';
		        return r;	       
		} else {
			*return_str = *str;
			str++;
			return_str++;
		}
	}
	return r;

}


// Part 1 Functions to implement
ModFile* PutModFile(int ins, int dels, char* filename, ModFile* mf) {

	if (mf == 0) {
		mf = (ModFile*)malloc(sizeof(ModFile));
		mf->filename = myStrCpy(filename, NULL);
		mf->inserts = 0;
		mf->deletes = 0;
	} else if (myStrCmp(mf->filename, filename) || filename == 0 || ins < 0 || dels < 0) {
	
		return NULL;
	}

	mf->inserts += ins;
	mf->deletes += dels;
	
	return mf;
}

int ModFileABC_Comparator(const void* file1, const void* file2) {
	ModFile *f1;
	ModFile *f2;
	f1 = (ModFile *)file1;
	f2 = (ModFile *)file2;

	int cmp = myStrCmp(f1->filename, f2->filename); 
	return cmp;
}

int ModFileTotal_Comparator(const void* file1, const void* file2) {
    	ModFile *f1;
	ModFile *f2;
	f1 = (ModFile *)file1;
	f2 = (ModFile *)file2;

	if ((f1->inserts + f1->deletes) < (f2->inserts + f2->deletes)) {
		return -1;
	} else if ((f1->inserts + f1->deletes) > (f2->inserts + f2->deletes)) {
		return 1;
	}else {
		return 0;
	}
}	

// Part 2 Functions to implement
void ModFile_Printer(void* data, void* fp, int flag) {
	if (data == NULL) {
		return;
	}

	ModFile *d;
	d = (ModFile *)data;

	fprintf((FILE *)fp, "Total Inserts:%d\tTotal Deletes:%d\t%s\n", d->inserts, d->deletes, d->filename);
}

void ModFile_Deleter(void* data) {
	ModFile *d;
	d = (ModFile *)data;

	free(d->filename);
}

node_t* FindInList(list_t* list, void* token)  {
   	
	if (list == 0 || token == 0) {
		return NULL;
	}
	
	node_t *n = list->head;
	if (n == 0) {
		return NULL;
	}
	ModFile *d = n->data;

	while (1) {
		if (n == 0) {
		       return NULL;
		}	       
		if (list->comparator(d, token) == 0) {
			return n;
		}
	        n = n->next;
		if (n == 0) {
			return NULL;
		}
	        d = n->data;
	}
}

void DestroyList(list_t** list) {

	if (*list == 0) {
		return;
	}

	if ((*list)->length == 0) {
		free(*list);
		return;
	}

	node_t *n = (*list)->head;
	node_t *old = n;
	ModFile *d = n->data;

	while(1) {
		if (n == 0) {
			free(*list);
			return;
		} else {
			(*list)->deleter(d);
			free(d);
		}

		n = n->next;
		free(old);
		old = n;
		if (n == 0) {
			free(*list);
			return;
		}
		d = n->data;
	}
}

void ProcessModFile(FILE* fp, list_t* list, char ordering) {
	if (list == NULL) {
		return;
	}

	int inserts;
	int deletes;
	char *filename = (char*)malloc(500 * sizeof(char));
	node_t *search;
	int rvals;

	while (1) {

		rvals = fscanf(fp, "%d\t%d\t%s", &inserts, &deletes, filename);

		if (rvals != 3) {
			break;
		}
				
		ModFile *new = PutModFile(inserts, deletes, filename, NULL);
		
		search = FindInList(list, new);
		if (search != NULL) {
			PutModFile(inserts, deletes, filename, search->data);
			list->deleter(new);
			free(new);
		} else {
			if (ordering == 'f') {
				InsertAtTail(list, new);
			} else if (ordering == 'a') {
				InsertInOrder(list, new);
			}
		}
	}
	free(filename);
	return;
}


// Part 3 Functions to implement
void AuthorPrinter(void* data, void *fp, int flag) {
	if (data == 0) {
		return;
	}

	Author *d = (Author *)data;
	list_t *list = d->modFileList;
	if (list == 0) {
		return;
	}
	node_t *node = list->head;
	if (node == 0) {
		return;
	}
	ModFile *mf = node->data;
	if (mf == 0) {
		return;
	}

	if (flag == 0) {
		fprintf((FILE *)fp, "%s <%s>,%d,%d\n", d->fullname, d->email, d->commitCount, list->length);
	} else {
		fprintf((FILE *)fp, "%s <%s>,%d\n", d->fullname, d->email, d->commitCount);
		while (1) {
			fprintf((FILE *)fp, "\tTotal Inserts:%d\tTotal Deletes:%d\t%s\n", mf->inserts, mf->deletes, mf->filename);
			node = node->next;
			if (node == 0) {
				return;
			}
			mf = node->data;
		}
	}	
}

int AuthorEmailComparator(const void* lhs, const void* rhs)  {
    	if (lhs == 0 || rhs == 0) {
		return 0;
	}

	Author *a1 = (Author *)lhs;
	Author *a2 = (Author *)rhs;

	return myStrCmp(a1->email, a2->email);
}

int AuthorCommitComparator(const void* lhs, const void* rhs) {
    	if (lhs == 0 || rhs == 0) {
		return 0;
	}
	
	Author *a1 = (Author *)lhs;
	Author *a2 = (Author *)rhs;

	if (a1->commitCount < a2->commitCount) {
		return 1;
	} else if (a1->commitCount > a2->commitCount) {
		return -1;
	} else {
		return 0;
	}
}

void AuthorDeleter(void* data)  {
	if (data == 0) {
		return;
	}

	Author *d = (Author *)data;

	free(d->fullname);
	free(d->email);
	
	list_t **delList;
	delList = &(d->modFileList);
	
	DestroyList(delList);
	delList = NULL;

}

Author* CreateAuthor(char* line, long int *timestamp)  {
	char *commitHash = malloc(500);
	Author *a = malloc(sizeof(Author));
	a->fullname = malloc(500);
	a->email = malloc(500);
	a->commitCount = 1;
	a->modFileList = CreateList(&ModFileABC_Comparator, &ModFile_Printer, &ModFile_Deleter);
	
	if (sscanf(line, "%500[^,]%*c%ld%*c%500[^,]%*c%500[^,]", commitHash, timestamp, a->fullname, a->email) != 4) {
		free(commitHash);
		free(a->fullname);
		free(a->email);
		DestroyList(&(a->modFileList));
		free(a);	
		return NULL;
	}

	free(commitHash);
	return a;
}


// Part 4 Functions to implement
void PrintNLinkedList(list_t* list, FILE* fp, int NUM) {
	
	node_t *n = list->head;
	if (n == 0) {
		return;
	}
	
	if (NUM == 0) {
		while(1) {
			list->printer(n->data, fp, 1);
			n = n->next;
			if (n == 0) {
				return;
			}
		}
	} else {
		for(int i = 0; i < NUM; i++) {
			list->printer(n->data, fp, 1);
			n = n->next;
			if (n == 0) {
				return;
			}
		}
	}
}


