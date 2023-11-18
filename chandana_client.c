#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_INPUT_LENGTH 200
#define STOP_SIGNAL "STOP"

int main() {
    char *ip = "127.0.0.1";
    int port = 5000;
    char answer[200];
    int sock;
    struct sockaddr_in server_addr;
    char buffer[MAX_INPUT_LENGTH];
    int cflag=0;
    printf("Enter source path, destination path, and action (copy_file or copy_folder): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Removing trailing newline character
    // if (strstr(buffer,"WRITE")==0)
    // {
        
    // }
    
    // Error handling for socket() call
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket error");
        exit(1);
    }
    printf("TCP client socket created.\n");

    // Initialize server address structure
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Connect to the naming server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection error");
        exit(1);
    }
    printf("Connected to the naming server.\n");

    // Send file paths and action request to the naming server
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        perror("Send error");
        exit(1);
    }
    printf("Source, destination paths, and action sent to the naming server.\n");

    // Receive response from the naming server (Storage Server IP and port)
    char ss_ip[INET_ADDRSTRLEN];
    int ss_port;

    if (recv(sock, ss_ip, INET_ADDRSTRLEN, 0) < 0) {
        perror("Receive error");
        exit(1);
    }
    if (recv(sock, &ss_port, sizeof(int), 0) < 0) {
        perror("Receive error");
        exit(1);
    }

    printf("Received Storage Server IP: %s, Port: %d\n", ss_ip, ss_port);

    // Close the socket to the naming server
    // close(sock);

    // Establish connection with the Storage Server
    struct sockaddr_in ss_server_addr;
    int ss_sock;

    ss_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ss_sock < 0) {
        perror("SS Socket error");
        exit(1);
    }

    memset(&ss_server_addr, '\0', sizeof(ss_server_addr));
    ss_server_addr.sin_family = AF_INET;
    ss_server_addr.sin_port = htons(ss_port);
    ss_server_addr.sin_addr.s_addr = inet_addr(ss_ip);

    if (connect(ss_sock, (struct sockaddr *)&ss_server_addr, sizeof(ss_server_addr)) < 0) {
        perror("SS Connection error");
        exit(1);
    }
    printf("Connected to the Storage Server.\n");

    // Receive data from the Storage Server until STOP signal or specific condition
    char ss_data[MAX_INPUT_LENGTH];
    while (1) {
        ssize_t bytes_received = recv(ss_sock, ss_data, sizeof(ss_data), 0);
        if (bytes_received < 0) {
            perror("Receive error");
            exit(1);
        } else if (bytes_received == 0) {
            printf("Connection closed by Storage Server.\n");
            break;
        } else {
            ss_data[bytes_received] = '\0';
            if (strcmp(ss_data, STOP_SIGNAL) == 0) {
                printf("Received STOP signal. Task completed.\n");
                break;
            }
            printf("Received data from Storage Server: %s\n", ss_data);
            // Process the received data or perform required operations
            // Here, you can handle the received data accordingly
        }
    }
    // char text_to_write[1024];
    // strcpy(text_to_write,"This is a test message for the storage server.");
    // send(ss_sock,buffer,sizeof(buffer),0);
    // // Close the socket to the Storage Server
    // close(ss_sock);

    return 0;
}
