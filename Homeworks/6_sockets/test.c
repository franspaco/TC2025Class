#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

    char thing[] = "1 2\n";

    int v1, v2;

    sscanf(thing, "%i", &v1);

    printf("%i\n", v1);

    sscanf(thing, "%i", &v1);

    printf("%i\n", v1);

    sscanf(thing, "%i %i", &v1, &v2);

    printf("%i %i\n", v1, v2);

    return 0;
}