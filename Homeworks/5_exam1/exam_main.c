

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "vigenere.h"
#include "vigenere_worker.h"

// Define buffer size to read from pipe
#define PIPE_BUFFER_SIZE 512

/**
 * Flush a stream,
 * useful to flush stdin
 */
void myflush ( FILE *in ){
    int ch;
    do
        ch = fgetc ( in ); 
    while ( ch != EOF && ch != '\n' ); 
    clearerr ( in );
}

/**
 * Prompts the user with a "Press enter to continue" screen
 */
void pressToContinue(){
    printf("[Press enter to continue]\n");
    myflush(stdin);
    fflush(stdout);
    getchar();
}

/**
 * This reads from the pipe and prints to stdout
 * Requires a file descriptor.
 */
void readPipe(int file_descriptor[])
{
    FILE * file_ptr = NULL;
    char buffer[PIPE_BUFFER_SIZE];

    // Close the descriptor for writing
    close(file_descriptor[1]);
    // Open the actual file pointer
    file_ptr = fdopen(file_descriptor[0], "r");

    // Get the data from the stream
    while ( fgets(buffer, PIPE_BUFFER_SIZE, file_ptr) )
    {
        printf("\n[INFO] The result was saved in: %s\n", buffer);
    }

    // Close the file pointer
    fclose(file_ptr);
    // Close the file descriptor
    close(file_descriptor[0]);
}


/**
 * Main function
 * takes no arguments
 */
int main(){

    // Keep promting the user
    while(1){
        char action;
        printf("\nSelect an option:\nEncode or Decode? (E/D)\nQuit (Q)\n");
        action = getchar();
        // Flush to clear extra characters that can lead to annoying cycles 
        myflush(stdin);
        switch(action){
            case 'D':
            case 'd':
                action = 0; // DECODING IS ZERO
                break;
            case 'E':
            case 'e':
                action = 1; // ENCODING IS ONE
                break;
            case 'Q':
            case 'q':
                printf("Quitting!\n");
                return 0;
            default:
                printf("Invalid option!\n");
                continue;
        }

        // FILE to read from
        printf("Enter the file name:\n");
        char filename[50];
        scanf("%s", filename);
        FILE* src_file = fopen(filename, "r");

        // Try to open the file
        if(src_file == NULL){
            // Error opening the file
            // Report error to user
            printf("File not found!\n");
            pressToContinue();
            continue;
        }

        // KEY
        printf("Please provide a key:\n");
        char key[50];
        scanf("%s", key);

        // PIPE
        int fd_c2f[2];
        if(pipe(fd_c2f)== -1){
            // Error creating the pipe
            printf("Error!\nQuitting!\n");
            exit(EXIT_FAILURE);
        }

        // FORKING
        pid_t pid;
        pid = fork();

        if(pid == 0){
            // Call the child's worker function
            worker_start(action, key, filename, src_file, fd_c2f);
            exit(0);
        }
        else if (pid > 0){
            // Read from the pipe and flush anything that might be left in stdin
            readPipe(fd_c2f);
            myflush(stdin);
        }
        else {
            // Very bad error
            printf("Error!\nQuitting!\n");
            exit(EXIT_FAILURE);
        }

    }
    
    return 0;
}

