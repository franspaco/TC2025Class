

#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <pthread.h>

#include "bank_codes.h"
#include "sockets.h"
#include "server_definitions.h"

// Data for a single bank account
typedef struct account_struct {
    int id;
    float balance;
} account_t;

// Data for the bank operations
typedef struct bank_struct {
    // Store the total number of operations performed
    int total_transactions;
    // An array of the accounts
    account_t * account_array;
} bank_t;

// Structure for the mutexes to keep the data consistent
typedef struct locks_struct {
    // Mutex for the number of transactions variable
    pthread_mutex_t transactions_mutex;
    // Mutex array for the operations on the accounts
    pthread_mutex_t * account_mutex;
} locks_t;

// Data that will be sent to each structure
typedef struct data_struct {
    // The file descriptor for the socket
    int connection_fd;
    // A pointer to a bank data structure
    bank_t * bank_data;
    // A pointer to a locks structure
    locks_t * data_locks;
} thread_data_t;

/**
 * CUSTOM STRUCTS
 */
// Used to pass connection data to the threads
typedef struct conenction_data {
    bank_t * bank_data;
    locks_t * data_locks;
    int fd;
} conenction_data_t;

// Used to contain incomming messages
typedef struct incomming_struct {
    int account;
    operation_t op;
    float value;
} inmsg_t;

// User to contain data after withrawing
typedef struct withraw_status_struct {
    float balance;
    int error;
} with_status_t;

/**
 * FUNCTIONS
 */

/**
 * Parses the received string into an easy to manipulate struct
 */
void parseIncomming(inmsg_t* output, char* s);

/**
 * A wrapper to facilitate sending responses
 */
void sendResponse(int connection_fd, response_t code, float balance);


#endif