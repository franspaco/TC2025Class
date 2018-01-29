
#include <stdio.h>
#include <stdlib.h>

int main() {
    init:
    printf("1");
    for(int i = 0; i < 10; i++){
        for1:
        printf("2");
        if(i > 0){
            printf("3");
            goto for2;
        }
    }

    for(int i = 0; i < 10; i++){
        for2:
        printf("4");
        if(i > 0){
            printf("5");
            goto for1;
        }
    }
}