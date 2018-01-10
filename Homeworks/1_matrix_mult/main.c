
/*
    Francisco Huelsz Prince
    A01019512

    HW1 - Matrix Multiplication
*/


#include <stdio.h>

struct M{
    int h;
    int w;
    float** m;
}typedef Matrix;

Matrix* readMatrix(char * fileName){
    FILE* fp = fopen(fileName, "r");
    Matrix newM;
    fscanf(fp, "%i %i", &(newM.h), &(newM.w));
    fclose(fp);
    printf("%i %i\n", newM.h, newM.w);
    float fl;
    fscanf(fp,"%f ", &fl);
    printf("%f\n", fl);
    return &newM;
}

int main(){
    printf("Hello world\n");
    readMatrix("Matrix_A.txt");
    return 0;
}