#include "helpers.h"
#include "debug.h"

/* Helper function definitions go here */
int my_ceil(int n, int v) {
    if (n == v) {
        return 1;
    }
    if ((n%v) == 0) {
        return n/v;
    } else {
        return (n/v) + 1;
    }
}