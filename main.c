#include <stdio.h>
#include "random437.h"

int main() {
    printf("Hello, World!\n");
    int myRandInt = poissonRandom(45);
    printf("My Random Number = %d\n", myRandInt);
    return 0;
}
