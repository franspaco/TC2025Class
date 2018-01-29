
#include <stdio.h>

#include "ppm.h"

int main(int argc, char** argv){
    ppm_t* img = ppm_readFile("test.ppm");
    ppm_print(img);
    ppm_invertColors(img);
    ppm_print(img);
    ppm_free(img);
    return 0;
}