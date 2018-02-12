
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "vigenere.h"

int main(){

    char test1[] = "Hello, world!";
    char key1[]  = "key";
    
    char* res = vigenere_encode(key1, test1);
    printf("%s\n", res);

    char* ori = vigenere_decode(key1, res);
    printf("%s\n", ori);
    free(res);
    free(ori);
    return 0;
}