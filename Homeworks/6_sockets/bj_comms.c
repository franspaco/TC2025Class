
#include "bj_comms.h"

/**
 * Send code to server
 */
void sendCode(int connection_fd, bj_code code){
    char buffer[BUFFER_SIZE];
    // Prepare the request to the server
    sprintf(buffer, "%i", code);
    //printf("Sending:\n%s\n", buffer);
    if (send(connection_fd, buffer, strlen(buffer) + 1, 0) == -1 ) {
        fatalError("send");
    }
}

/**
 * Send code to server with trailer data
 */
void sendCodeTrailer(int connection_fd, bj_code code, int trailer){
    char buffer[BUFFER_SIZE];
    // Prepare the request to the server
    sprintf(buffer, "%i %i", code, trailer);
    //printf("Sending:\n%s\n", buffer);
    if (send(connection_fd, buffer, strlen(buffer) + 1, 0) == -1 ) {
        fatalError("send");
    }
}

int awaitResponse(int connection_fd, int* code, int* data){
    char buffer[BUFFER_SIZE];
    int chars_read;

    chars_read = recv(connection_fd, buffer, BUFFER_SIZE, 0);

    if(chars_read ==-1 || chars_read == 0){
        *code = -1;
        *data = -1;
        return chars_read;
    }

    //printf("%s\n", buffer);

    sscanf(buffer, "%i", code);

    if(hasTrailer(*code)){
        sscanf(buffer, "%i %i", code, data);
    }
    else{
        *data = -1;
    }
    return chars_read;
}