// Your helper functions need to be here.
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int cd(const char *path) {
//	fprintf(stdout, "%s\n", path);
	int r = chdir(path);
	return r;
}
