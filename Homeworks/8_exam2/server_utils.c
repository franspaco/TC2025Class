/**
 * Francisco Huelsz Prince
 * A01019512
 */

#include "server_utils.h"
/**
 * CUSTOM FUNCTIONS ARE HERE
 */

void parseIncomming(inmsg_t* msg, char* s){
    sscanf(s, "%i %i %f", (int*)&(msg->op), &(msg->account), &(msg->value));
}

void sendResponse(int connection_fd, response_t code, float balance){
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    sprintf(buffer, "%i %f", (int)code, balance);
    sendString(connection_fd, buffer);
}