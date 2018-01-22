
/*
    Francisco Huelsz Prince
    A01019512

    HW2 - Base conversions
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/**
 * PREPROCESSOR DEFINITIONS
 */
// Needs to be a large value to store big strings/big numbers
#define STRLEN 35

// Type definitions
typedef union D{
    int i;
    char s[STRLEN];
} Data;

// Function definitions
void convert(int fr, int to, Data* valv, int valc);
void itoa(int val, char* dest, int str_size, int radix);


int main(int argc, char ** argv){
    int option = 0;
    int fr = -1;
    int to = -1;

    // Parse options
    while((option = getopt(argc, argv, "i:o:")) != -1 ){
        switch(option){
            case 'i':
                fr = atoi(optarg);
                break;
            case 'o':
                to = atoi(optarg);
                break;
            default:
                break;
        }
    }
    int valc = argc - optind;
    if(valc < 1 || fr < 2 || to < 2 || fr > 36 || to > 36){
        printf("Invalid options!\nCheck your input.\n");
        exit(1);
    }

    // Parse values to convert
    Data* valv = malloc(valc * sizeof(Data));
    for (int index = optind; index < argc; index++){
        if(fr == 10){
            valv[index - optind].i = atoi(argv[index]);
        }
        else {
            strncpy(valv[index - optind].s, argv[index], STRLEN);
        }
    }

    // Repeat input to user
    for(int i = 0; i < valc; i++){
        if(fr == 10){
            printf("%i\n", valv[i].i);
        }
        else {
            printf("%s\n", valv[i].s);
        }
    }
    printf("Converting from: %i to %i\n", fr, to);

    // Convert
    convert(fr, to, valv, valc);

    // Print output
    for(int i = 0; i < valc; i++){
        if(to == 10){
            printf("%i\n", valv[i].i);
        }
        else {
            printf("%s\n", valv[i].s);
        }
    }
    free(valv);
    return 0;
}

void convert(int fr, int to, Data* valv, int valc){
    // Just in case input and output base are the same
    if(fr == to)
        return;
    // Turn everything into base 10
    if(fr != 10){
        for(int i = 0; i < valc; i++){
            valv[i].i = strtol(valv[i].s, NULL, fr);
        }
    }

    // Everything is in base 10 now, so return if this is desired
    if(to == 10)
        return;
    
    // Transform each value into another base
    for(int i = 0; i < valc; i++){
        itoa(valv[i].i, valv[i].s, STRLEN, to);
    }
}

/**
 * Converts an int into any base and outputs the result to a string.
 */
void itoa(int val, char* dest, int str_size, int radix){
    // Characters
    char vals[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Check if the requested output base is possible
    if(radix < 2 || radix > 36){
        printf("ERROR: Invalid base!\nABORTING!\n");
        *dest = '\0';
        return;
    }
    dest[str_size-1] = '\0';
    int indx = str_size-2;
    for(; val > 0; indx--){
        if(indx < 0){
            printf("ERROR: Overflow! Increase string length (STRLEN)\nABORTING!\n");
            *dest = '\0';
            return;
        }
        dest[indx] = vals[val % radix];
        val /= radix;
    }
    if(dest != &dest[indx + 1])
        strcpy(dest, &dest[indx + 1]);
}