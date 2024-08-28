#include <stdio.h>
#include <stdlib.h>
#include <string.h> //string.h functions are not allowed in your assignment

#include "../include/helpers2.h"
#include "../include/hw2.h"
#include "../include/linkedlist.h"

int main(void) {

    printf("\n****Can't modify the arguments*****\n");
    char* sampleStr = "Who?\n What!? Why???";   //reference to static string
    char* delims = "";                       //reference to static string
    
    char* result = myStrCpy(sampleStr, delims);

    printf("sampleStr: %s\nresult: %s, result address: %p\n\n", sampleStr, result, result);   

    printf("\n****Did you modify the arguments???*****\n");
    char sampleStr2[] = "Who?\n What!? Why???";   //reference to static string
    char delims2[] = "?\n";                       //reference to static string
    
    char* result2 = myStrCpy(sampleStr2, delims2);

    printf("sampleStr2: %s\nresult: %s, result address: %p\n\n", sampleStr2, result2, result2);   

    if(strcmp(sampleStr, sampleStr2) != 0) //string.h functions are not allowed in your assignment!!!
        printf("OOPS!!! sampleStr2 was modified - Don't do that!\n");

    if(strcmp(delims, delims2) != 0) //string.h functions are not allowed in your assignment!!!
        printf("OOPS!!! delims2 was modified - Don't do that!\n");

    return 0;
}
