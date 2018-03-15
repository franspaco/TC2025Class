


#ifndef BJ_COMMS_H
#define BJ_COMMS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>

#include "fatal_error.h"
#include "blackjack.h"

#define BUFFER_SIZE 1024



void sendCode(int connection_fd, bj_code code);
void sendCodeTrailer(int connection_fd, bj_code code, int trailer);
int awaitResponse(int connection_fd, int* code, int* data);


#endif