
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "vigenere_worker.h"

int main() {

    char filename[] = "file.txt";

    char* thing = create_outfile_name(filename, 1);

    printf("%lu\n", strlen(thing));
    printf("%s\n", thing);
    printf("%i\n", thing[strlen(thing)-1]);

    return 0;
}