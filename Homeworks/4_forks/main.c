
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

    int num = -1;
    while(1){
        printf("\nEnter number:\n");
        scanf("%i", &num);
        if(num < 0){
            printf("Quitting!\n");
            break;
        }
        printf("Calculating %i!\n", num);
        int pid = fork();
        if(pid == 0){
            // Child process
            char number[10];
            sprintf(number, "%i", num);
            char* program = "./factorial";
            execl(program, program, number, (char*)NULL );
        }
        else if(pid > 0){
            // Parent
            int status;
            waitpid(pid, &status, 0);
            int res = WEXITSTATUS(status);
            printf("%i\n", res);
        }
        else {
            printf("Fatal error!\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}