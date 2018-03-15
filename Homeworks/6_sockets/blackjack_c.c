/**
 * Francisco Huelsz Prince
 * A01019512
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>
// Custom libraries
#include "fatal_error.h"
#include "blackjack.h"
#include "bj_comms.h"

#include <ifaddrs.h>

#define BUFFER_SIZE 1024

///// FUNCTION DECLARATIONS
void usage(char * program);
int openSocket(char * address, char * port);

void startGame(int connection_fd);
int doAction(int connection_fd, int code, int data, hand_t* hand);


void myflush ( FILE *in ){
    int ch;
    do
        ch = fgetc ( in ); 
    while ( ch != EOF && ch != '\n' ); 
    clearerr ( in );
}

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int connection_fd;

    printf("\n=== Blackjack client ===\n");

    // Check the correct arguments
    if (argc != 3)
    {
        usage(argv[0]);
    }

    // Start the connection
    connection_fd = openSocket(argv[1], argv[2]);
	
    startGame(connection_fd);

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
    Open the socket to the server
    Returns the file descriptor for the socket
    Remember to close the socket when finished
*/
int openSocket(char * address, char * port)
{
    struct addrinfo hints;
    struct addrinfo * server_info = NULL;
    int connection_fd;

    // Prepare the hints structure
    bzero (&hints, sizeof hints);
    // Set to use IPv4
    hints.ai_family = AF_INET;
    // Set type of socket
    hints.ai_socktype = SOCK_STREAM;
    // Set to look for the address automatically
    hints.ai_flags = AI_PASSIVE;

    // GETADDRINFO
    // Use the presets to get the actual information for the socket
    if (getaddrinfo(address, port, &hints, &server_info) == -1)
    {
        fatalError("getaddrinfo");
    }

    // SOCKET
    // Open the socket using the information obtained
    connection_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (connection_fd == -1)
    {
        close(connection_fd);
        fatalError("socket");
    }

    // CONNECT
    // Connect to the server
    if (connect(connection_fd, server_info->ai_addr, server_info->ai_addrlen) == -1)
    {
        fatalError("connect");
    }

    // FREEADDRINFO
    // Free the memory used for the address info
    freeaddrinfo(server_info);

    return connection_fd;
}

void startGame(int connection_fd){
    printf("Enter your bet: (int)\n");
    int bet;
    scanf("%i", &bet);
    sendCodeTrailer(connection_fd, NEW, bet);
    hand_t hand;
    hand.num = 0;
    hand.sum = 0;

    while(1){
        int code;
        int data;
        int bytes_read = awaitResponse(connection_fd, &code, &data);

        if(bytes_read == -1 || bytes_read == 0){
            fatalError("Receive!\n");
        }

        // Must return 1 to keep in game, 0 to quit.
        if(!doAction(connection_fd, code, data, &hand)){
            return;
        }

    }

    sendCode(connection_fd, QUIT);
}

void printHand(hand_t* hand){
    printf("Your hand has %i cards:\n", hand->num);
    for(int i = 0; i < hand->num; i++){
        printf(" %i", hand->cards[i]);
    }
    printf("\nSum: %i\n", hand->sum);
}


int doAction(int connection_fd, int code, int data, hand_t* hand){
    switch(code){
        case DEAL:
            if(data == -1){
                printf("Oh no!\nYou asked for too many cards!\n");
                return 0;
            }
            printf("The dealer has given you a: %i\n", data);
            hand->cards[hand->num++] = data;
            hand->sum += data;
            printHand(hand);

            if(hand->sum > 21){
                printf("Oh no!\n YOU LOST!!\n");
                sendCode(connection_fd, LOST);
                return 0;
            }

            if(hand->num == 1){
                sendCode(connection_fd, HIT);
                return 1;
            }
            sendCode(connection_fd, INQUIRY_DEALER);
            return 1;
        case DEALER_ACTION:
            if(data == 0){
                printf("Dealer took a secret card!\n");
            }
            else {
                printf("Dealer drew a %i\n", data);
            }
        case DEALER_READY:
            printf("Dealer did nothing more.\n");
            printf("Do you wish to hit, stand or quit game? (h/s/q)\n");
            char opt;
            scanf("\n%c", &opt);
            //printf("USER INPUT IS: %i\n", opt);
            if(opt == 'h' || opt == 'H'){
                printf("You've chosen to: HIT\n");
                sendCode(connection_fd, HIT);
            }
            else if(opt == 's' || opt == 'S'){
                printf("You've chosen to: STAND\n");
                sendCode(connection_fd, STAND);

            }
            else if(opt == 'q' || opt == 'Q'){
                printf("Quitting game!\n");
                return 0;
            }
            else{
                printf("Invalid option!!\n");
                sendCode(connection_fd, QUIT);
                fatalError("USER");
            }
            return 1;
        case DEALING_FINISHED:
            if(data > 0){
                // PLAYER WINS
                printf("YOU WON!\n");
            }
            else if(data < 0){
                // DEALER WINS
                printf("YOU LOST!\n");
            }
            else {
                // TIE
                printf("YOU IT's A TIE!\n");
            }
            printHand(hand);
            sendCode(connection_fd, GET_DEALER_CARDS);
            return 1;
        case READY_TO_SEND_CARDS:
            sendCode(connection_fd, NEXT_CARD);
            printf("Dealer has: \n");
            return 1;
        case DEALER_CARD:
            printf(" %i", data);
            sendCode(connection_fd, NEXT_CARD);
            return 1;
        case DEALER_DONE:
            printf("\n");
            return 0;
        case QUIT:
            return 0;
        default:
            fatalError("CODE");
    }
    return 0;
}
