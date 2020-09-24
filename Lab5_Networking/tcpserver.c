// Compile with: gcc tcpserver.c -o tcpserver
//
// Implement tcp server below.
// Our standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Some types and unix operations
#include <sys/types.h>
#include <unistd.h>

// A sockets library and internet protocol library
#include <netinet/in.h>
#include <sys/socket.h>

#include <signal.h>

// Create the server socket
int server_socket;

// Create a signal handler
void sigint_handler(int sig) {
	// Close the socket
	close(server_socket);
	write(1, "Terminating through signal handler\n", 35);
	exit(0);
}

#define BF_SZ 256 

int main() {
	char name[BF_SZ] = "";
	char recv_buf[BF_SZ] = "";
	// memset(recv_buf, '\0', sizeof(recv_buf));
	// Install our signal handler.
	signal(SIGINT, sigint_handler);
	//			domain, type 	    ,protocol
	// domain = AF_INET(IPv4) or AF_INET6
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// define the server address format
	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	// 9002 is choosen arbitrarily. Try other ports if this does not work
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = htons(INADDR_ANY);

	bind(server_socket, (struct sockaddr *)&server_address,
	     sizeof(server_address));
	printf("Server bind()\n");

	listen(server_socket, 27);
	printf("Server Listening\n");
	while (1) {
		// Integer to hold a potential client socket
		int client_socket;
		client_socket = accept(server_socket, NULL, NULL);
		printf("Client_socket %d\n", client_socket);

		// send a message from the server
		char server_message[BF_SZ] = "You have reached the server";

		send(client_socket, server_message, sizeof(server_message), 0);
		// Recv username
		int ret = recv(client_socket, name, sizeof(name), 0);
		if (ret > 0) {
			printf("> %s\n", name);
		}
		while (ret > 0) {
			ret = recv(client_socket, recv_buf, sizeof(recv_buf), 0);
			printf("> %s\n", recv_buf);
		}
		printf("Client_socket %d disconnceted\n", client_socket);

		close(client_socket);
	}
}
