/*
    Program for a simple bank server
    It uses sockets and threads
    The server will process simple transactions on a limited number of accounts

    Gilberto Echeverria
    gilecheverria@yahoo.com
    29/03/2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Signals library
#include <errno.h>
#include <signal.h>
// Sockets libraries
#include <netdb.h>
#include <sys/poll.h>
// Posix threads library
#include <pthread.h>

// Custom libraries
#include "bank_codes.h"
#include "sockets.h"
#include "fatal_error.h"

// Custom^2 Libraries
#include "server_utils.h"
#include "server_definitions.h"



///// FUNCTION DECLARATIONS
void usage(char * program);
void setupHandlers();
void initBank(bank_t * bank_data, locks_t * data_locks);
void waitForConnections(int server_fd, bank_t * bank_data, locks_t * data_locks);
void * attentionThread(void * arg);
int checkValidAccount(int account);
void closeBank(bank_t * bank_data, locks_t * data_locks);

// Custom functions
float getBalance(conenction_data_t* data, int account_num);
float depositToAccount(conenction_data_t* data, int account_num, float ammount);
void withrawFromAccount(with_status_t* status, conenction_data_t* data, int account_num, float amount);
int getTransactionsInThread(conenction_data_t* data);
int getTransactions(bank_t* bank_data, pthread_mutex_t* trans_lock);
void exitHandler(int signal);


// Global vars
int exit_flag = 0;



///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int server_fd;
    bank_t bank_data;
    locks_t data_locks;

    printf("\n=== SIMPLE BANK SERVER ===\n");

    // Check the correct arguments
    if (argc != 2)
    {
        usage(argv[0]);
    }

    // Configure the handler to catch SIGINT
    setupHandlers();

    // Initialize the data structures
    initBank(&bank_data, &data_locks);

	// Show the IPs assigned to this computer
	printLocalIPs();
    // Start the server
    server_fd = initServer(argv[1], MAX_QUEUE);
	// Listen for connections from the clients
    waitForConnections(server_fd, &bank_data, &data_locks);
    // Close the socket
    close(server_fd);

    // Clean the memory used
    closeBank(&bank_data, &data_locks);

    // Finish the main thread
    pthread_exit(NULL);

    return 0;
}

///// FUNCTION DEFINITIONS

/*
    Explanation to the user of the parameters required to run the program
*/
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {port_number}\n", program);
    exit(EXIT_FAILURE);
}

/*
    Modify the signal handlers for specific events
*/
void setupHandlers() {
    signal(SIGINT, exitHandler);
}


/**
 * SIGINT handler
 */
 void exitHandler(int signal){
     // Set flag to true
     exit_flag = 1;
 }


/*
    Function to initialize all the information necessary
    This will allocate memory for the accounts, and for the mutexes
*/
void initBank(bank_t * bank_data, locks_t * data_locks)
{
    // Set the number of transactions
    bank_data->total_transactions = 0;

    // Allocate the arrays in the structures
    bank_data->account_array = malloc(NUM_ACCOUNTS * sizeof (account_t));
    // Allocate the arrays for the mutexes
    data_locks->account_mutex = malloc(NUM_ACCOUNTS * sizeof (pthread_mutex_t));

    // Initialize the mutexes, using a different method for dynamically created ones
    //data_locks->transactions_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&data_locks->transactions_mutex, NULL);
    for (int i=0; i<NUM_ACCOUNTS; i++)
    {
        //data_locks->account_mutex[i] = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_init(&data_locks->account_mutex[i], NULL);
    }
}

/*
    Main loop to wait for incomming connections
*/
void waitForConnections(int server_fd, bank_t * bank_data, locks_t * data_locks)
{
    struct sockaddr_in client_address;
    socklen_t client_address_size;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    pthread_t new_tid;
    int poll_response;
	int timeout = 100;		// Time in milliseconds (0.5 seconds)

    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;

    while (1)
    {
		//// POLL
        // Create a structure array to hold the file descriptors to poll
        struct pollfd test_fds[1];
        // Fill in the structure
        test_fds[0].fd = server_fd;
        test_fds[0].events = POLLIN;    // Check for incomming data
        // Check if there is any incomming communication
        poll_response = poll(test_fds, 1, timeout);

		// Error when polling
        if (poll_response == -1)
        {
            // SIGINT will trigger this
            // errno is checked to make sure it was a signal that got us here and not an error
            if(errno == EINTR && exit_flag){
                printf("\n\n[ALERT] SERVER SHUTTING DOWN! - No longer listening.\n");
                // An exit signal got us here, therefore we must break
                break;
            }
            else{
                // Some other error got us here
                fatalError("poll");
            }
        }
		// Timeout finished without reading anything
        else if (poll_response == 0){

            // The exit flag has been activated => stop listening for requests
            if(exit_flag){
                printf("\n\n[ALERT] SERVER SHUTTING DOWN! - No longer listening.\n");
                break;
            }
            //printf("No response after %d seconds\n", timeout);
        }
		// There is something ready at the socket
        else
        {
            // Check the type of event detected
            if (test_fds[0].revents & POLLIN)
            {
				// ACCEPT
				// Wait for a client connection
				client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);
				if (client_fd == -1)
				{
					fatalError("ERROR: accept");
				}
				 
				// Get the data from the client
				inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
				printf("[INFO] Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);

				// Prepare the structure to send to the thread

				// CREATE A THREAD
                conenction_data_t* temp_data = malloc(sizeof(conenction_data_t));
                temp_data->bank_data = bank_data;
                temp_data->data_locks = data_locks;
                temp_data->fd = client_fd;
                int status;
                status = pthread_create(&new_tid, NULL, attentionThread, (void*) temp_data);
                if( status != 0){
                    printf("[ERROR] Failed to create handler!\n");
                }
                else{
                    printf("[INFO] Created thread %lu for request.\n", new_tid);
                }
            }
        }
    }

    // Print the total number of transactions performed
    printf("[INFO] Bank closed with: %i transactions.\n", getTransactions(bank_data, &(data_locks->transactions_mutex)));

}

/*
    Hear the request from the client and send an answer
*/
void * attentionThread(void * arg) {
    conenction_data_t* data = (conenction_data_t*) arg;
    printf("[INFO][%lu] Attending request!\n", pthread_self());

    // Stuff required by poll
    struct pollfd test_fds[1];
    int timeout = 10; // 10ms tiemout
    int poll_result;

    // buffer
    char buffer[BUFFER_SIZE];

    //Account stuff
    float balance = 0;
    with_status_t withraw_status;

    while (!exit_flag) {
        // POLL
        // Fill in the data for the structure
        test_fds[0].fd = data->fd;
        test_fds[0].events = POLLIN;
        // Call poll
        poll_result = poll(test_fds, 1, timeout);
        
        if (poll_result == -1) {
            // SIGINT will trigger this
            // errno is checked to make sure it was a signal that got us here and not an error
            if(errno == EINTR && exit_flag){
                printf("POLL KILLED BY FLAG!\n");
                // An exit signal got us here, therefore we must break
                break;
            }
            else{
                // Some other error got us here
                fatalError("poll");
            }
        }
        else if (poll_result == 0){
            // Nothing
        }
        else {
            // Got a message
            if(recvString(data->fd, buffer, BUFFER_SIZE) == 0){
                printf("[INFO][%lu] Client disconnected!\n", pthread_self());
                break;
            }
            //printf("[INFO][%lu] Got text: %s\n",pthread_self(), buffer);
            inmsg_t msg;
            parseIncomming(&msg, buffer);

            // Client exiting procedure
            if(msg.op == EXIT){
                printf("[INFO][%lu] Client is leaving!\n", pthread_self());
                break;
            }

            // Make sure account is valid, throw error otherwise
            if(!checkValidAccount(msg.account)){
                sendResponse(data->fd, NO_ACCOUNT, 0);
                continue;
            }

            // Last resort exit flag
            // In case it was activated during reading
            // Only happens in very intense workloads
            // (Like my testing metodology described in the README)
            if(exit_flag){
                break;
            }

            switch(msg.op){
                case CHECK:
                    // Get balance
                    balance = getBalance(data, msg.account);
                    break;
                case DEPOSIT:
                    // Make sure value is valid
                    if(msg.value < 0){
                        sendResponse(data->fd, ERROR, 0);
                        continue;
                    }
                    // Make deposit
                    balance = depositToAccount(data, msg.account, msg.value);
                    break;
                case WITHDRAW:
                    // Make sure value is valid
                    if(msg.value < 0){
                        sendResponse(data->fd, ERROR, 0);
                        continue;
                    }
                    // Try widthraw
                    withrawFromAccount(&withraw_status, data, msg.account, msg.value);
                    if(withraw_status.error){
                        // Widthraw error
                        sendResponse(data->fd, INSUFFICIENT, 0);
                        continue;
                    }
                    else{
                        // Widthraw success
                        balance = withraw_status.balance;
                    }
                    break;
                default:
                    fatalError("Unknown code!");
            }

            // If we got here no errors were raised: send OK with balance
            sendResponse(data->fd, OK, balance);
            printf("[INFO][%lu] Successful transaction! - Total: %i\n", pthread_self(), getTransactionsInThread(data));
        }
    }
    // Inform client that the server is closing
    sendResponse(data->fd, BYE, 0);

    // Free the struct used to pass massages onto the thread
    free(data);

    printf("[INFO][%lu] Closing thread!\n", pthread_self());
    pthread_exit(NULL);
}

/*
    Free all the memory used for the bank data
*/
void closeBank(bank_t * bank_data, locks_t * data_locks) {
    printf("[DEBUG] Clearing the memory for the thread\n");
    free(bank_data->account_array);
    free(data_locks->account_mutex);
}

/*
    Return true if the account provided is within the valid range,
    return false otherwise
*/
int checkValidAccount(int account)
{
    return (account >= 0 && account < NUM_ACCOUNTS);
}

/**
 * Custom functions
 */

float getBalance(conenction_data_t* data, int account_num){
    // Extract all the values so the code is more readable
    account_t* account = &(data->bank_data->account_array[account_num]);
    pthread_mutex_t* account_lock = &(data->data_locks->account_mutex[account_num]);
    pthread_mutex_t* trans_lock = &(data->data_locks->transactions_mutex);
    float value;

    // Locks account first and transactions second
    // Does everything that's required 
    // Unlocks transactions first and account second
    // This order is meant to avoid deadlocks
    pthread_mutex_lock(account_lock);
    pthread_mutex_lock(trans_lock);

    // This just reads
    value = account->balance;
    data->bank_data->total_transactions++;

    pthread_mutex_unlock(trans_lock);
    pthread_mutex_unlock(account_lock);

    return value;
}

float depositToAccount(conenction_data_t* data, int account_num, float amount){
    // Extract all the values so the code is more readable
    account_t* account = &(data->bank_data->account_array[account_num]);
    pthread_mutex_t* account_lock = &(data->data_locks->account_mutex[account_num]);
    pthread_mutex_t* trans_lock = &(data->data_locks->transactions_mutex);
    float value;

    // Locks account first and transactions second
    // Does everything that's required 
    // Unlocks transactions first and account second
    // This order is meant to avoid deadlocks
    pthread_mutex_lock(account_lock);
    pthread_mutex_lock(trans_lock);

    // This just adds 2 numbers
    account->balance += amount;
    value = account->balance;
    data->bank_data->total_transactions++;
    pthread_mutex_unlock(trans_lock);
    pthread_mutex_unlock(account_lock);

    return value;
}

void withrawFromAccount(with_status_t* status, conenction_data_t* data, int account_num, float amount){
    // Extract all the values so the code is more readable
    account_t* account = &(data->bank_data->account_array[account_num]);
    pthread_mutex_t* account_lock = &(data->data_locks->account_mutex[account_num]);
    pthread_mutex_t* trans_lock = &(data->data_locks->transactions_mutex);

    // Locks account first and transactions second
    // Does everything that's required 
    // Unlocks transactions first and account second
    // This order is meant to avoid deadlocks
    pthread_mutex_lock(account_lock);
    pthread_mutex_lock(trans_lock);

    // Make sure enough funds are available, act accordingly and report errors if raised
    if(account->balance >= amount){
        account->balance -= amount;
        data->bank_data->total_transactions++;
        status->balance = account->balance;
        status->error = 0;
    }
    else{
        status->balance = 0;
        status->error = 1;
    }
    pthread_mutex_unlock(trans_lock);
    pthread_mutex_unlock(account_lock);
}

/**
 * Wrapper to get transactions from within the threads
 */
int getTransactionsInThread(conenction_data_t* data){
    return getTransactions(data->bank_data, &(data->data_locks->transactions_mutex));
}

/**
 * Gets Transactions
 */
int getTransactions(bank_t* bank_data, pthread_mutex_t* trans_lock){
    int value;
    pthread_mutex_lock(trans_lock);
    value = bank_data->total_transactions;
    pthread_mutex_unlock(trans_lock);
    return value;
}

