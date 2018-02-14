

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "vigenere_worker.h"

int main() {

    char filename[] = "encoded_test.txt";
    FILE* file = fopen(filename, "r");
    printf("10\n");
    worker_start(0, "key", filename, file, NULL);

    return 0;
}