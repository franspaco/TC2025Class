/**
 * Francisco Huelsz Prince
 * A01019512
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>
// Custom libraries
#include "fatal_error.h"
#include "blackjack.h"
#include "bj_comms.h"

#include <ifaddrs.h>

#define BUFFER_SIZE 1024
#define MAX_QUEUE 5

///// FUNCTION DECLARATIONS
void usage(char * program);
void printLocalIPs();
int initServer(char * port);
void waitForConnections(int server_fd);
void attendRequest(int client_fd);

void startGame(int client_fd);
int doAction(int connection_fd, int code, int data, hand_t* hand, hand_t* dealer);

///// MAIN FUNCTION
int main(int argc, char * argv[]){
    int server_fd;

    printf("\n=== BLACKJACK SERVER ===\n");

    // Check the correct arguments
    if (argc != 2)
    {
        usage(argv[0]);
    }

	// Show the IPs assigned to this computer
	printLocalIPs();

    // Start the server
    server_fd = initServer(argv[1]);
	// Listen for connections from the clients
    waitForConnections(server_fd);
    // Close the socket
    close(server_fd);

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
	Show the local IP addresses, to allow testing
	Based on code from:
		https://stackoverflow.com/questions/20800319/how-do-i-get-my-ip-address-in-c-on-linux
*/
void printLocalIPs()
{
	struct ifaddrs * addrs;
	struct ifaddrs * tmp;
	
	// Get the list of IP addresses used by this machine
	getifaddrs(&addrs);
	tmp = addrs;

    printf("Server IP addresses:\n");

	while (tmp) 
	{
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET)
		{
		    // Get the address structure casting as IPv4
			struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            // Print the ip address of the local machine
			printf("%s: %s\n", tmp->ifa_name, inet_ntoa(pAddr->sin_addr));
		}
		// Advance in the linked list
		tmp = tmp->ifa_next;
	}

	freeifaddrs(addrs);
}

/*
    Prepare and open the listening socket
    Returns the file descriptor for the socket
    Remember to close the socket when finished
*/
int initServer(char * port)
{
    struct addrinfo hints;
    struct addrinfo * server_info = NULL;
    int server_fd;
    int reuse = 1;

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
    if (getaddrinfo(NULL, port, &hints, &server_info) == -1)
    {
        fatalError("getaddrinfo");
    }

    // SOCKET
    // Open the socket using the information obtained
    server_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (server_fd == -1)
    {
        close(server_fd);
        fatalError("socket");
    }

    // SETSOCKOPT
    // Allow reuse of the same port even when the server does not close correctly
    if (setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (int)) == -1)
    {
        fatalError("setsockopt");
    }

    // BIND
    // Connect the port with the desired port
    if (bind(server_fd, server_info->ai_addr, server_info->ai_addrlen) == -1)
    {
        fatalError("bind");
    }

    // LISTEN
    // Start listening for incomming connections
    if (listen(server_fd, MAX_QUEUE) == -1)
    {
        fatalError("listen");
    }

    // FREEADDRINFO
    // Free the memory used for the address info
    freeaddrinfo(server_info);

    printf("Server ready\n");

    return server_fd;
}

/*
    Main loop to wait for incomming connections
*/
void waitForConnections(int server_fd)
{
    struct sockaddr_in client_address;
    socklen_t client_address_size;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;

    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;

    while (1)
    {
        // ACCEPT
        // Wait for a client connection
        client_fd = accept(server_fd, (struct sockaddr *) &client_address, &client_address_size);
        if (client_fd == -1)
        {
            fatalError("accept");
        }
         
        // Get the data from the client
        inet_ntop (client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
        printf("[INFO] Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);
        
        // Deal with the client
        //attendRequest(client_fd);

        pid_t pid = fork();
        if(pid > 0){
            // Parent
            printf("[INFO] Child %i will handle request!\n", pid);
            continue;
        }
        else if(pid == 0){
            // Child
            //attendRequest(client_fd);
            startGame(client_fd);
            printf("[INFO] [%i] shutting down!\n", getpid());
            exit(0);
            printf("NEVER PRINTS\n");
        }
        else{
            printf("[CRASH] FORK FAILED!\n");
            fatalError("Process creation failed!");
        }
       
    }
}


void startGame(int client_fd){
    int code = -1, data = -1;
    char buffer[BUFFER_SIZE];

    int bytes_read;

    hand_t hand;
    hand.num = 0;
    hand.sum = 0;

    hand_t dealer;
    dealer.num = 0;
    dealer.sum = 0;

    while(1){
        // Clear the buffer to avoid errors
        bzero(&buffer, BUFFER_SIZE);

        bytes_read = awaitResponse(client_fd, &code, &data);

        if(bytes_read == 0){
            printf("[INFO] [%i] Client disconnected\n", getpid());
            return;
        }
        if (bytes_read == -1)
        {
            printf("[ERROR] [%i] Client receive error\n", getpid());
            return;
        }

        printf("[INFO] [%i] Client sent: %i %i\n", getpid(), code, data);

        // Do the thing
        if(!doAction(client_fd, code, data, &hand, &dealer)){
            printf("[INFO] [%i] Client disconnected\n", getpid());
            return;
        }
    }
}

int dealerPlay(hand_t* dealer){
    if(dealer->sum < 17){
        int card = rand() % 9 + 1;
        dealer->cards[dealer->num++] = card;
        dealer->sum += card;
        if(dealer->num > 1){
            return 0;
        }
        else{
            return card;
        }
    }
    return -1;
}

int card_index = 0;

// Does the required action
// Returns 1 to continue cycle
// Returns 0 to quit cycle
int doAction(int connection_fd, int code, int data, hand_t* hand, hand_t* dealer){
    srand(time(NULL));
    switch(code){
        case NEW:
            hand->bet = data;
        case HIT:
            if(hand->num == HAND_SIZE){
                sendCodeTrailer(connection_fd, DEAL, -1);
                return 0;
            }
            int card = rand() % 9 + 1;
            sendCodeTrailer(connection_fd, DEAL, card);
            hand->cards[hand->num++] = card;
            hand->sum += card;
            return 1;
        case INQUIRY_DEALER:
            ;
            int dealer_played = dealerPlay(dealer);
            if(dealer_played >= 0){
                sendCodeTrailer(connection_fd, DEALER_ACTION, dealer_played);
            }
            else{
                sendCode(connection_fd, DEALER_READY);
            }
            return 1;
        case STAND:
            while(dealerPlay(dealer) != -1){}

            hand->sum = abs(21 - hand->sum);
            dealer->sum = abs(21 - dealer->sum);

            if(hand->sum < dealer->sum){
                // PLAYER WINS
                sendCodeTrailer(connection_fd, DEALING_FINISHED, 1);
            }
            else if(hand->sum < dealer->sum){
                // DEALER WINS
                sendCodeTrailer(connection_fd, DEALING_FINISHED, -1);
            }
            else{
                // TIE
                sendCodeTrailer(connection_fd, DEALING_FINISHED, 0);
            }
            return 1;
        case GET_DEALER_CARDS:
            card_index = 0;
            sendCode(connection_fd, READY_TO_SEND_CARDS);
            return 1;
        case NEXT_CARD:
            if(card_index < dealer->num){
                sendCodeTrailer(connection_fd, DEALER_CARD, dealer->cards[card_index++]);
            }
            else{
                sendCode(connection_fd, DEALER_DONE);
            }
            return 1;
        case LOST:
        printf("[INFO] [%i] Client lost! Received bet of $%i\n", getpid(), hand->bet);
            return 0;
        case QUIT:
            return 0;
        default:
            printf("[ERROR] [%i] Client sent unexpected code\n", getpid());
            return 0;
    }
}


/*
    Hear the request from the client and send an answer
*/
void attendRequest(int client_fd)
{
    char buffer[BUFFER_SIZE];
    int chars_read;
    unsigned long int iterations;
    double result;
    
    // Clear the buffer to avoid errors
    bzero(&buffer, BUFFER_SIZE);

    // RECV
    // Read the request from the client
    chars_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (chars_read == 0)
    {
        printf("[INFO] [%i] Client disconnected\n", getpid());
        return;
    }
    if (chars_read == -1)
    {
        printf("[ERROR] [%i] Client receive error\n", getpid());
        return;
    }
    // Get the numerical value for iterations
    sscanf(buffer, "%lu", &iterations);

    printf("[INFO] [%i] Got request from client with iterations=%lu\n", getpid(), iterations);

    result = 3.14159;

    // SEND
    // Write back the reply
    sprintf(buffer, "%.20lf\n", result);
    if (send(client_fd, buffer, strlen(buffer) + 1, 0) == -1)
    {
        printf("[ERROR] [%i] Could not send reply!\n", getpid());   
    }
}
