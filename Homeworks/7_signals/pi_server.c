/*
    Server program to compute the value of PI
    This program calls the library function 'get_pi'
    It will create child processes to attend requests
    It receives connections using sockets

    Gilberto Echeverria
    gilecheverria@yahoo.com
    21/02/2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>

#include <sys/poll.h>
#include <signal.h>
#include <errno.h>

// Custom libraries
#include "fatal_error.h"

#include <ifaddrs.h>

#define BUFFER_SIZE 1024
#define MAX_QUEUE 5

int exit_flag = 0;
int attending_request = 0;

///// FUNCTION DECLARATIONS
void usage(char * program);
void printLocalIPs();
int initServer(char * port);
void waitForConnections(int server_fd);
void attendRequest(int client_fd);
void exitHandler(int signal);

///// MAIN FUNCTION
int main(int argc, char * argv[]) {

    signal(SIGINT, exitHandler);

    int server_fd;

    printf("\n=== SERVER FOR COMPUTING THE VALUE OF pi ===\n");

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

void exitHandler(int sig){
    // Notify of the situation
    printf("\nQUITTING!\n");
    // Set up exit flag to true
    exit_flag = 1;
    if(attending_request == 0){
        // This will kill any process that is not directly dealing with 
        //   a client, namely the parent process, and therefore has 
        //   nothing else to do.
        exit(0);
    }
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
    pid_t new_pid;

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
        
        // Create a child to deal with the new client
        new_pid = fork();
        // Parent process
        if (new_pid > 0)
        {
            // Close the socket to the new client
            close(client_fd);
        }
        // Child process
        else if (new_pid == 0)
        {
            // Get the data from the client
            inet_ntop (client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
            printf("Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);
            
            // Deal with the client
            attendRequest(client_fd);
            
            // Finish the child process
            close(client_fd);
            exit(EXIT_SUCCESS);
        }
        // Error
        else
        {
            fatalError("Unable to fork");
        }
    }
}

/*
    Hear the request from the client and send an answer
*/
void attendRequest(int client_fd) {
    // Set attending_request flag to true
    attending_request = 1;

    char buffer[BUFFER_SIZE];
    int chars_read;
    unsigned long int iterations;
    
    // Clear the buffer to avoid errors
    bzero(&buffer, BUFFER_SIZE);

    // RECV
    // Read the request from the client
    chars_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (chars_read == 0)
    {
        printf("Client disconnected");
        return;
    }
    if (chars_read == -1)
    {
        printf("Client receive error");
        return;
    }
    // Get the numerical value for iterations
    sscanf(buffer, "%lu", &iterations);

    printf(" > Got request from client with iterations=%lu\n", iterations);

    // Stuff required by poll
    struct pollfd test_fds[1];
    int timeout = 10; // 10ms tiemout
    int poll_result;

    // PI calculation control
    int current_it = 0;
    int max_it_per_turn = 10000;

    // PI calculation stuff
    double pi_result = 4;
    int sign = -1;
    unsigned long int divisor = 3;

    while (1) {
        // POLL
        // Fill in the data for the structure
        test_fds[0].fd = client_fd;
        test_fds[0].events = POLLIN;
        // Call poll
        poll_result = poll(test_fds, 1, timeout);
        
        if (poll_result == -1) {
            // SIGINT will trigger this, to return the current value we just need to break
            // out of the while-loop
            // errno is checked to make sure it was a signal that got us here and not an error
            if(errno == EINTR){
                // An exit signal got us here, therefore we must break to return pi
                break;
            }
            else{
                // Some other error got us here
                fatalError("poll");
            }
        }
        else if (poll_result == 0){
            // If nothing has happened calculate up to max_it_per_turn iterations of PI
            for(int i = 0; i < max_it_per_turn; i++){
                if(current_it > iterations){
                    // If the target # of iterations has been met send result
                    goto send;
                }
                pi_result += sign * (4.0/divisor);
                sign *= -1;
                divisor += 2;
                current_it++;
            }
        }
        else {
            // Should anything be received break
            // This means the client is quitting and has requested 
            //  the current value of PI to be returned
            if (test_fds[0].revents & POLLIN) {
                break;
            }
        }
    }

    send:
    // SEND
    // Write back the reply
    sprintf(buffer, "%.20lf\n", pi_result);
    if (send(client_fd, buffer, strlen(buffer) + 1, 0) == -1)
    {
        printf("Could not send reply");   
    }
}
