
/**
 * Francisco Huelsz Prince
 * A01019512
 * 
 * factorial.c
 * 
 * compile with:
 *  gcc factorial.c -o factorial
 * For it to work properly with main.c
 */


#include <stdio.h>
#include <stdlib.h>

/**
 * Gets an int
 * returns the factorial of said int,
 * possible shortcomings: returns 1 for anything below 2,
 * correct for 1 and 0, but not for -1 and all the negativity that follows
 */
int calculateFactorial(int num){
    int res = 1;
    while(num > 1){
        res *= num;
        num--;
    }
    return res;
}

/**
 * This program MUST be called with just ONE (1) argument.
 * The argument MUST be an INT.
 */
int main(int argc, char ** argv){
    // Not enough or too many arguments
    if(argc != 2){
        //printf("0\n");
        return 0;
    }
    // Parse parameter to int
    int a = atoi(argv[1]);

    // Make sure it's doable
    if(a >= 0){
        int result = calculateFactorial(a);
        //printf("%i\n", result);
        return result;
    }
    return 0;
}