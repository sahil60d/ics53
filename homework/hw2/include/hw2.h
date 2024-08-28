// DO NOT MODIFY THIS FILE
// Any additions should be placed in helpers2.h

#ifndef HW_2_H
#define HW_2_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "linkedlist.h"

typedef struct 
{
	unsigned int inserts;
	unsigned int deletes;
	char* filename;
} ModFile;

typedef struct 
{      
      char*   fullname; 
      char*   email;
      int     commitCount;    // number of commits made by user
      list_t* modFileList; // list of ModFile structs ordered alphabetically by filename
} Author;

// Part 0 Function to implement
char* myStrCpy(char* str, char* delimiters);

// Part 1 Functions to implement
ModFile* PutModFile(int ins, int dels, char* filename, ModFile* mf);
int ModFileABC_Comparator(const void* file1, const void* file2);
int ModFileTotal_Comparator(const  void* file1, const void* file2);

// Part 2 Functions to implement
void ModFile_Printer(void* data, void* fp, int flag);
void ModFile_Deleter(void* data);
node_t* FindInList(list_t* list, void* token);
void DestroyList(list_t** list);
void ProcessModFile(FILE* fp, list_t* list, char ordering);

// Part 3 Functions to implement
void AuthorPrinter(void* data, void *fp, int flag);
int AuthorEmailComparator(const void* lhs, const void* rhs);
int AuthorCommitComparator(const void* lhs, const void* rhs);
void AuthorDeleter(void* data);
Author* CreateAuthor(char* line, long int* timestamp);


// Part 4 Functions to implement
void PrintNLinkedList(list_t* list, FILE* fp, int NUM);





int is_delim (char c, char *delim);



#endif
