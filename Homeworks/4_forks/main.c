
/**
 * Francisco Huelsz Prince
 * A01019512
 * 
 * main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(){

    printf("Factorial calculator!\nEnter a number to calculate it's factorial,\nEnter -1 to quit.\n");

    // User input look
    int num = -1;
    while(1){
        printf("\nEnter number:\n");
        scanf("%i", &num);
        if(num < 0){
            printf("Quitting!\n");
            break;
        }
        printf("Calculating %i!\n", num);

        // Create child
        int pid = fork();
        if(pid == 0){
            // Child process
            // Turn number into char[]
            char number[10];
            sprintf(number, "%i", num);
            char* program = "./factorial";
            // change to factorial program with the argument as number
            execl(program, program, number, (char*)NULL );
        }
        else if(pid > 0){
            // Parent

            // Wait for child
            int status;
            waitpid(pid, &status, 0);

            // Get result and print
            int res = WEXITSTATUS(status);
            printf("%i\n", res);
        }
        else {
            // Error
            printf("Fatal error!\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}