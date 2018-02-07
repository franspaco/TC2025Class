
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int calculateFactorial(int num){
    int res = 1;
    while(num > 1){
        res *= num;
        num--;
    }
    return res;
}

int main(){

    printf("Factorial calculator!\nEnter a number to calculate it's factorial,\nEnter -1 to quit.\n");

    int num = -1;
    while(1){
        printf("Enter number:\n");
        scanf("%i", &num);
        if(num < 1){
            printf("Quitting!\n");
            break;
        }
        printf("Calculating %i!\n", num);
        int pid = fork();
        if(pid == 0){
            int res = calculateFactorial(num);
            printf("Res: %i\n", res);
            return res;
            break;
        }
        else if(pid > 0){
            int status;
            waitpid(pid, &status, 0);
            printf("Status: %i\n", status);
        }
        else {
            printf("Fatal error!\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}