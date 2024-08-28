#include <stdio.h>
#include <stdlib.h>

#include "helpers2.h"
#include "hw2.h"
#include "linkedlist.h"

int main(void) {


    printf("\n****Create new ModFile*****\n");
    char modfile_line1[] = "2\t1\tMarsSplashScreen.java";  // edittable copy of string

    printf("Modified File line is: %s\n", modfile_line1);
    ModFile* f1 = PutModFile(2,1,"MarsSplashScreen.java", NULL);
	//printf("%d, %d", f1->inserts, f1->deletes);
    if(f1 == NULL)
        printf("OOPS!!! Failed to allocate a new ModFile Struct\n");
    else
        printf("YEAH!!! Created space on the heap for a new ModFile instance\n");



    printf("\n****Update created Modfile*****\n");
    char* modfile_file1 = "MarsSplashScreen.java";  //reference to static string
    ModFile* f2 = PutModFile(20,100,modfile_file1, f1);

    if(f1 != f2)
        printf("OOPS!!! Not the same struct passed to the function\n");
    else if (f2 != NULL)
        printf("inserts:%u, deletes:%u, filename:%s\n", f2->inserts, f2->deletes, f2->filename);
    else
        printf("OOPS!!! What did you do?!?!\n");




    printf("\n****Fail to Update created Modfile*****\n");
    char* modfile_file2 = "MarsSplashScreen.txt";  //reference to static string
    f2 = PutModFile(10,1,modfile_file2, f1);

    if(f2 != NULL)
        printf("OOPS!!! No Match, don't update the struct.\ninserts:%u, deletes:%u, filename:%s\n", f2->inserts, f2->deletes, f2->filename);
    else
        printf("YEAH!!! Failed to match the filename\n");



    printf("\n****Compare ModFiles by ABC*****\n");
    ModFile f3 = {411,0,"ztestfile.pdf"};   // f3 struct is initialized with static string
    ModFile f4 = {1,7,"testfile.pdf"};   // f3 struct is initialized with static string
    int success = ModFileABC_Comparator(&f3, &f4);

    if(success < 0)
        printf("Modfile f3 (%s) is before ModFile f4 (%s)\n", f3.filename, f4.filename);
    else if (success > 0)
        printf("Modfile f3 (%s) is after ModFile f4 (%s)\n", f3.filename, f4.filename);
    else
        printf("Modfile f3 (%s) equal to ModFile f4 (%s)\n", f3.filename, f4.filename);


    printf("\n****Compare ModFiles by Total*****\n");
    success = ModFileTotal_Comparator(&f3, &f4);

    if(success < 0)
        printf("Modfile f3 (%s) has less modifications than ModFile f4 (%s)\n", f3.filename, f4.filename);
    else if (success > 0)
        printf("Modfile f3 (%s) has more modifications than  ModFile f4 (%s)\n", f3.filename, f4.filename);
    else
        printf("Modfile f3 (%s) has equal number of modification as ModFile f4 (%s)\n", f3.filename, f4.filename);





    return 0;
}
