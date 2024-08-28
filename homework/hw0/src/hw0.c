// Sahil Desai
// sjdesai



#include "hw0.h"

int main (int argc, char *argv[])
{

	//Comment/Delete this print statement and insert your hw0 code here 
	//printf("Hello ICS53 student!\n"); 
	
	int counter = argc - 1;
	while(counter > -1){
		printArg(argv[counter], counter);
		counter--;
	}	

	return 0;
}

//Function to print out a single arugment to the screen
void printArg(char * arg_str, int pos){

	printf("Argument[%d]: %s\n", pos, arg_str);

	return;
}
