// Note: I recommend implementing the tcpserver.c first.
//
// Compile with: gcc tcpclient.c -o tcpclient
//
// Implement tcp client below.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <unistd.h>

#include <signal.h>

#define BF_SZ 256

// Create a socket
int client_socket;

// For getline.
char *send_buf = NULL;
size_t size = 0;

void reset_send_buf() {
    free(send_buf);
    send_buf = NULL;
}

// Create a signal handler
void sigint_handler(int sig) {
    reset_send_buf();
    // Close the socket
    close(client_socket);
    write(1, "Terminating through signal handler\n", 35);
    exit(0);
}


int main() {
    do {
        printf("What is your username:");
        getline(&send_buf, &size, stdin);
        send_buf[strcspn(send_buf, "\n")] = '\0';
    } while (strlen(send_buf) < 1);


    signal(SIGINT, sigint_handler);
    // Note that the protocol matches our server here
    // Also note I am doing some error checking for my socket.
    // You may consider updating your server code to do the same.
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
    }

    // Our client will connect to a server with the following address and port.
    struct sockaddr_in server_address;
    // 32-bit IPV4 address
    server_address.sin_family = AF_INET;
    // convert to the appropriate port number using htons
    server_address.sin_port = htons(9172);
    // Our ip address, which is localhost using INADDR_ANY
    server_address.sin_addr.s_addr = htons(INADDR_ANY);

    // Attempt to connect
    // Returns an integer as an error code (zero means everything worked)
    int connection_status;

    // Report back to user if our connection was successful
    if ((connection_status =
                 connect(client_socket, (struct sockaddr *) &server_address,
                         sizeof(server_address))) < 0) {
        printf("%d Connection unsuccessful\n", connection_status);
        close(client_socket);
        exit(0);
    } else {
        printf("Connected successfully\n");
        send(client_socket, send_buf, strlen(send_buf), 0);
        reset_send_buf();
    }

    // Receive data from the server
    char server_response[BF_SZ];
    int ret = recv(client_socket, &server_response, sizeof(server_response), 0);

    // Print out the server's response
    printf("The server sent the data: %s\n", server_response);
    while (ret) {
        printf("client>");
        getline(&send_buf, &size, stdin);
        send_buf[strcspn(send_buf, "\n")] = '\0';
        if (strcmp("exit", send_buf) == 0) {
            reset_send_buf();
            printf("Client stops.\n");
            break;
        }

        int send_sz = send(client_socket, send_buf, strlen(send_buf) + 1, 0);
        reset_send_buf();
        if (send_sz <= 0) {
            printf("Failed to send the message to the server\n");
            break;
        }
    }
    // Then close the socket
    close(client_socket);
    return 0;
}
