
/*
    Francisco Huelsz Prince
    A01019512

    HW1 - Matrix Multiplication

    How to use:
    Run the program and provide both matrices' file names as arguments, 
    and a name to save the resulting matrix, eg.:

    ./program Matrix1.txt Matrix2.txt result.txt

    Which will calculate Matrix1 X Matrix2 and write it to result.txt, if possible.
*/


#include <stdio.h>
#include <stdlib.h>

/**
 * Matrix struct
 * Contains:
 *  - Height (int)
 *  - Width (int)
 *  - Matrix (float**)
 */
typedef struct M{
    int h;
    int w;
    float** m;
} Matrix;


/**
 * Prints a matrix m
 * Receives a pointer to a fully allocated matrix
 */
void printMatrix(Matrix * m){
    for(int i = 0; i < m->h; i++){
        for(int j = 0; j < m->w; j++){
            printf("%f ", (m->m)[i][j]);
        }
        printf("\n");
    }
}

/**
 * Reads a file and returns a pointer to a matrix containing the data read from the file.
 * Receives a char* containing the filename
 */
Matrix* readMatrix(char * fileName){
    // Open specified file
    FILE* fp = fopen(fileName, "r");

    // Create new matrix
    Matrix * newM;
    newM = (Matrix*)malloc(sizeof (Matrix));

    // Read size
    char line[500];
    fgets(line, sizeof line, fp);
    char *ptr;
    newM->h = strtol(line, &ptr, 10);
    newM->w = strtol(ptr, &ptr, 10);

    // Allocate memory
    newM->m = (float**)malloc((newM->h) * sizeof(float*));
    for(int i = 0; i < newM->h; i++){
        newM->m[i] = (float*)malloc((newM->w) * sizeof(float));
    }

    // Fill in data
    for(int i = 0; i < newM->h; i++){
        fgets(line, sizeof line, fp);
        ptr = line;
        for(int j = 0; j < newM->w; j++){
            (newM->m)[i][j] = strtof(ptr, &ptr);
        }
    }

    // Close file and return pointer
    fclose(fp);
    return newM;
}

/**
 * Checks if multiplying A x B is possible
 *  - returns 1 is possible
 *  - returns 0 if impossible
 */
int canMultiply(Matrix* a, Matrix* b){
    return (a->w) == (b->h);
}

/**
 * Multiplies AxB
 * Returns a pointer to a new Matrix
 */
Matrix* multiply(Matrix* a, Matrix* b){

    // Create new Matrix
    Matrix * newM;
    newM = (Matrix*)malloc(sizeof (Matrix));

    newM->h = a->h;
    newM->w = b->w;

    newM->m = (float**)malloc((newM->h) * sizeof(float*));
    for(int i = 0; i < newM->h; i++){
        newM->m[i] = (float*)malloc((newM->w) * sizeof(float));
    }

    // Multiply and fill in values in the output matrix
    for(int la = 0; la < a->h; la++){
        for(int cb = 0; cb < b->w; cb++){
            float sum = 0;
            for(int el = 0; el < a->w; el++){
                sum += (a->m)[la][el] * (b->m)[el][cb];
            }
            (newM->m)[la][cb] = sum;
        }
    }

    return newM;
}

/**
 * Frees all the memory associated with a fully allocated matrix
 */
void freeMatrix(Matrix* m){
    for(int i = 0; i < m->h; i++){
        free ((m->m)[i]);
    }
    free (m->m);
    free (m);
}

/**
 * Write matrix to file.
 * Receives:
 *  - Pointer to matrix
 *  - File name
 */
void writeMatrix(Matrix* m, char* name){
    FILE* fp = fopen(name, "w");
    fprintf(fp, "%i %i\n", m->h, m->w);
    for(int i = 0; i < m->h; i++){
        for(int j = 0; j < m->w; j++){
            fprintf(fp, "%f ", (m->m)[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    printf("\nResult written to \"%s\"\n", name);
}

/**
 * Main function
 * Receives two arguments:
 *  - Filename of matrix A
 *  - Filename of matrix B
 *  - Filename of matric C (Output)
 * 
 * Then tries to multiply AxB
 */
int main(int argc, char ** argv){

    if(argc != 4){
        printf("Argument error\n");
        exit(1);
    }

    // Read matrices
    printf("Reading: %s (A)\n", argv[1]);
    Matrix* a = readMatrix(argv[1]);
    printMatrix(a);

    printf("Reading: %s (B)\n", argv[2]);
    Matrix* b = readMatrix(argv[2]);
    printMatrix(b);

    printf("Multiplying A x B\n (%i, %i) X (%i, %i)\n", a->h, a->w, b->h, b->w);

    // Verify if possible
    if(canMultiply(a,b)){
        printf("\nResult:\n");
        Matrix* c = multiply(a, b);
        printMatrix(c);
        writeMatrix(c, argv[3]);
        freeMatrix(c);
    }
    else{
        printf("Multiplication cannot be done!\n");
    }
    freeMatrix(a);
    freeMatrix(b);
    return 0;
}