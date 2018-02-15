
/**
 * Francisco Huelsz Prince
 * A01019512
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "vigenere_worker.h"
#include "vigenere.h"


char* create_outfile_name(char* filename, char action){
    int len = strlen(filename);
    
    char* out = malloc((len + 8 + 1) * sizeof(char));

    if(action){
        // ENCODING
        strcpy(out, "encoded_");
    }
    else {
        // DECODING
        strcpy(out, "decoded_");
    }
    strcat(out, filename);

    return out;
}

void worker_start(char action, char* key, char* fileName, FILE* src_file, int* fd){
    // Close file descriptor for reading
    close(fd[0]);
    // Open file with file descriptor to write in
    FILE* pipe_file = fdopen(fd[1], "w");
    // Generate the name of the new file
    char* outfileName = create_outfile_name(fileName, action);
    // Open said file
    FILE* out = fopen(outfileName, "w");

    // Start reading from the file
    char buffer[BUFFER_SIZE];
    size_t nread;

    // Needed to keep track how many pieces there are
    int count = 0;
    while((nread = fread(buffer, 1, BUFFER_SIZE, src_file)) > 0){
        // Insert null character if less characters were read
        if(nread < 512)
            buffer[nread] = '\0';
        
        // Encode/Decode
        char* temp;
        if(action){
            // ENCODING
            temp = vigenere_encode_partial(key, buffer, count, BUFFER_SIZE);
        }
        else {
            // DECODING
            temp = vigenere_decode_partial(key, buffer, count, BUFFER_SIZE);
        }
        // Write to file
        fprintf(out, "%s", temp);

        // Free the created string.
        free(temp);

        // Count the chunk
        count++;
    }
    // Close files
    fclose(src_file);
    fclose(out);

    // Report back to main
    fprintf(pipe_file, "%s", outfileName);

    // Free allocated strings
    free(outfileName);

    // Close pipe
    fclose(pipe_file);
    close(fd[1]);
}