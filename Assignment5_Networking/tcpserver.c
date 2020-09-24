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
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define BF_SZ 256
#define MAX_CONNECTION 27

/* Command execution functions */
/*
Don't know why the assignment states "When a client receives a message, it will
fork() a new process and run the 'system' to try to run that command on the
server."
As the system() function will use fork to create a child process that execute
the command using execl. We can simply call system() in the main function.
System() will do the fork by itself. It seems there is no need to fork in the
main process and then call system in the child process. With that being said,
I have tried my best to implement the server exactly as the assignment
specified as long as the way specified in the assignment does work.
*/
void exec_cmd(char *buffer) {
	int status = 0;
	int pid = fork();
	if (pid == 0) {
		system(buffer);
		exit(EXIT_SUCCESS);
	}
	wait(&status);
	return;
}

// Create the server socket
int server_socket;

// Create a signal handler
void sigint_handler(int sig) {
	// Close the server socket.
	close(server_socket);
	write(1, "Terminating through signal handler\n", 35);
	exit(0);
}

int main() {

	char name[BF_SZ] = "";
	char recv_buf[BF_SZ] = "";
	// memset(recv_buf, '\0', sizeof(recv_buf));
	// Install our signal handler.
	signal(SIGINT, sigint_handler);
	//			domain, type 	    ,protocol
	// domain = AF_INET(IPv4) or AF_INET6
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	// In order to reuse the address after the program exit.
	int reuse = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,
	               (const char *)&reuse, sizeof(reuse)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
	}

	// define the server address format
	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	// 9002 is choosen arbitrarily. Try other ports if this does not work
	server_address.sin_port = htons(9172);
	server_address.sin_addr.s_addr = htons(INADDR_ANY);

	if (bind(server_socket, (struct sockaddr *)&server_address,
	         sizeof(server_address)) < 0) {
		perror("bind");
	} else {
		printf("Server: bind()\n");
	}

	if (listen(server_socket, MAX_CONNECTION) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	char server_message[BF_SZ] = "You have reached the server";

	printf("Server: Listening\n");
	while (1) {
		// New client requests to connect.
		// Integer to hold a potential client socket
		int client_socket;
		client_socket = accept(server_socket, NULL, NULL);
		printf("Server: Client_socket %d connected.\n", client_socket);

		// send a message from the server
		send(client_socket, server_message, strlen(server_message) + 1, 0);
		// Recv username
		memset(name, '\0', sizeof(name));
		int ret = recv(client_socket, name, sizeof(name), 0);
		if (ret > 0) {
			printf("Server: %s has joined the server. Welcome!\n", name);
		}

		while (1) {
			int ret = recv(client_socket, recv_buf, sizeof(recv_buf), 0);
			if (ret > 0) {
				// New message from current client.
				printf("%s> ", name);
				printf("%s\n", recv_buf);
				exec_cmd(recv_buf);
			} else {
				printf("Server: Client_socket %d disconnceted. ",
				       client_socket);
				printf("Goodbye! %s!\n", name);
				close(client_socket);
				break;
			}
		}
	}
}
