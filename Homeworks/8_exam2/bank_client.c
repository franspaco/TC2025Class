/**
 * Francisco Huelsz Prince
 * A01019512
 */

/*
    Client program to access the accounts in the bank
    This program connects to the server using sockets

    Gilberto Echeverria
    gilecheverria@yahoo.com
    29/03/2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>
// poll
#include <sys/poll.h>
// Custom libraries
#include "bank_codes.h"
#include "sockets.h"
#include "fatal_error.h"

#define BUFFER_SIZE 1024

///// FUNCTION DECLARATIONS
void usage(char * program);
void bankOperations(int connection_fd);

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int connection_fd;

    printf("\n=== BANK CLIENT PROGRAM ===\n");

    // Check the correct arguments
    if (argc != 3)
    {
        usage(argv[0]);
    }

    // Start the server
    connection_fd = connectSocket(argv[1], argv[2]);
	// Use the bank operations available
    bankOperations(connection_fd);
    // Close the socket
    close(connection_fd);

    return 0;
}

///// FUNCTION DEFINITIONS

/*
    Explanation to the user of the parameters required to run the program
*/
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {server_address} {port_number}\n", program);
    exit(EXIT_FAILURE);
}

/*
    Main menu with the options available to the user
*/
void bankOperations(int connection_fd)
{
    char buffer[BUFFER_SIZE];
    int account;
    float amount;
    float balance;
    char option = 'c';
    int status;
    operation_t operation;

    // Stuff required by poll stdin
    struct pollfd test_fds[1];
    int timeout = 10; // 10ms tiemout
    int poll_result;

    while (option != 'x'){
        printf("Bank menu:\n");
        printf("\tc. Check balance\n");
        printf("\td. Deposit into account\n");
        printf("\tw. Withdraw from account\n");
        printf("\tx. Exit program\n");
        printf("Select an option: \n");

        // So I wasn't happy that there was no way for the client to know the server had shut down,
        // therefore I made this double poll extravaganza that handles that case.
        // Polls both stdin and the socket and will end the client if the server went off.
        while(1){
            test_fds[0].fd = stdin->_fileno;
            test_fds[0].events = POLLIN;
            poll_result = poll(test_fds, 1, timeout);
            if(poll_result == -1){
                //??
                fatalError("STDIN poll");
            }
            else if(poll_result == 0){
                // Nothing: check for server replies
                test_fds[0].fd = connection_fd;
                test_fds[0].events = POLLIN;
                poll_result = poll(test_fds, 1, timeout);
                if(poll_result == -1){
                    //??
                    fatalError("Server poll");
                }
                else if(poll_result == 0){
                    // Nothing: continue polling
                    continue;
                }
                else{
                    // Got something, most likely a BYE: read and make sure
                    // Receive the response
                    if ( !recvString(connection_fd, buffer, BUFFER_SIZE) ){
                        printf("\n\nServer closed the connection\n");
                        exit(0);
                    }
                    // Extract the data
                    sscanf(buffer, "%d %f", &status, &balance);
                    if(status == BYE){
                        printf("\n\nServer is shutting down! BYE\n");
                        exit(0);
                    }
                }
            }
            else{
                scanf(" %c", &option);
                break;
            }
        }

        // Init variables to default values
        account = 0;
        amount = 0;
        balance = 0;

        switch(option)
        {
            // Check balance
            case 'c':
                printf("Enter account: ");
                scanf("%d", &account);
                operation = CHECK;
                break;
            // Deposit into account
            case 'd':
                printf("Enter account: ");
                scanf("%d", &account);
                printf("Enter the amount to deposit: ");
                scanf("%f", &amount);
                operation = DEPOSIT;
                break;
            // Withdraw from account
            case 'w':
                printf("Enter account: ");
                scanf("%d", &account);
                printf("Enter the amount to withdraw: ");
                scanf("%f", &amount);
                operation = WITHDRAW;
                break;
            // Exit the bank
            case 'x':
                printf("Thanks for using the program. Bye!\n");
                operation = EXIT;
                break;
            // Incorrect option
            default:
                printf("Invalid option. Try again ...\n");
                // Skip the rest of the code in the while
                continue;
        }

        // Prepare the message to the server
        sprintf(buffer, "%d %d %f", operation, account, amount);

        // SEND
        // Send the request
        sendString(connection_fd, buffer);

        // RECV
        // Receive the response
        if ( !recvString(connection_fd, buffer, BUFFER_SIZE) )
        {
            printf("Server closed the connection\n");
            break;
        }
        // Extract the data
        sscanf(buffer, "%d %f", &status, &balance);

        // Print the result
        switch (status)
        {
            case OK:
                printf("\tThe balance in account %d is %.2f\n", account, balance);
                break;
            case INSUFFICIENT:
                printf("\tInsufficient funds for the transaction selected\n");
                break;
            case NO_ACCOUNT:
                printf("\tInvalid acount number entered\n");
                break;
            case BYE:
                if(option == 'x'){
                    printf("\tThanks for connecting to the bank. Good bye!\n");
                }
                else{
                    printf("\n\nThe server closed before the action could be performed.\nBalance was not affected!\n");
                }
                exit(0);
                break;
            case ERROR: default:
                printf("\tInvalid operation. Try again\n");
                break;
        }
    }
}
