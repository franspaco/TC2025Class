

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(){
    pid_t my_pid;
    pid_t my_father;
    pid_t new_pid;

    new_pid = fork();

    my_pid = getpid();
    my_father = getppid();

    printf("I am:   %d\n", my_pid);
    printf("Parent: %d\n", my_father);

    return 0;
}