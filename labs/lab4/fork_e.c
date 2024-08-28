#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  pid_t pid;
  char c;

  FILE *fd = fopen("nums.txt", "r");
  if (fd == NULL) {
    printf("Error: Could not open file");
    exit(0);
  }

  pid = fork();
  if (pid == 0) {
    /*this is the child*/
    while ((c = getc(fd)) != 0xff) {
      scanf("child: %c\n", &c);
      //printf("child: %c\n", c);
    }
    exit(0);
  }

  /* parent code */
  while ((c = getc(fd)) != 0xff) {
    //printf("parent: %c\n", c);
    scanf("parent: %c\n", &c);
  }

  exit(0);
}
