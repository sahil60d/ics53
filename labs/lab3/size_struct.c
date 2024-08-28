#include <stdio.h>
#include <stdlib.h>

struct foo {
  char c[5];
};

struct foo1 {
  char c[5];
  short data;
};

struct foo2 {
  char c[5];
  int data;
};

struct foo3 {
  char c[5];
  long data;
};

struct foo4 {
  char c[5];
  long data;
  char x;
};

struct foo5 {
	char c;
	char pad1[7];
	struct foo5_inner {
		char *p;
		short x;
		char pad2[6];
	} inner;
};

int main(int argc, char **argv) {
  printf("sizeof(struct foo)  = %zu\n", sizeof(struct foo));
  printf("sizeof(struct foo1) = %zu\n", sizeof(struct foo1));
  printf("sizeof(struct foo2) = %zu\n", sizeof(struct foo2));
  printf("sizeof(struct foo3) = %zu\n", sizeof(struct foo3));
  printf("sizeof(struct foo4) = %zu\n", sizeof(struct foo4));
  printf("sizeof(struct foo5) = %zu\n", sizeof(struct foo5)); 
}
