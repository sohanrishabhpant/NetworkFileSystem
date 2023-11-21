#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
typedef char *string;
#define MAX_INPUT_LENGTH 200
#define STOP_SIGNAL "STOP"
typedef struct file_prop
{
    mode_t permissions;
    size_t file_size;
} file_prop;
char **gettokens(char *token)
{
    int count;
    char **argv = malloc(sizeof(char *) * 1000);
    for (int i = 0; i < 1000; i++)
    {
        argv[i] = malloc(sizeof(char) * 10000);
    }
    count = 0;

    char *arg = strtok_r(token, " \t", &token);
    while (arg != NULL)
    {
        argv[count++] = arg;
        char *msd = strtok_r(NULL, " \t", &token);
        arg = msd;
    }
    argv[count] = NULL;
    return argv;
}
int main()
{
    char *ip = "127.0.0.1";
    int port = 5000;
    char answer[200];
    int sock;
    struct sockaddr_in server_addr;
    char buffer[MAX_INPUT_LENGTH];
    int cflag = 0;
    int ss_sock;
    // printf("Enter source path, destination path, and action (copy_file or copy_folder): ");
    while (1)
    {

        // Error handling for socket() call
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
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
        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            perror("Connection error");
            exit(1);
        }
        printf("Connected to the naming server.\n");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Removing trailing newline character

        // Send file paths and action request to the naming server
        int flag = 1;
        if (send(sock, &flag, sizeof(int), 0) < 0)
        {
            perror("send error\n");
            exit(2);
        }
        if (send(sock, buffer, strlen(buffer), 0) < 0)
        {
            perror("Send error");
            exit(1);
        }
        printf("Source, destination paths, and action sent to the naming server.\n");
        // Receive response from the naming server
        char ack[1024];
        bzero(ack, 1024);
        ssize_t ack_len = recv(sock, ack, sizeof(ack) - 1, 0);
        if (ack_len < 0)
        {
            perror("recv error");
        }
        else
        {
            ack[ack_len] = '\0'; // Null-terminate the received data
            printf("Received ack from nm: %s\n", ack);
        }
        // listen(sock,1000);
        printf("recieved ack from nm: %s\n", ack);
        // Receive response from the naming server (Storage Server IP and port)
        char ss_ip[1000];
        int ss_port;
        char **argv = malloc(sizeof(char *) * 1000);
        for (int i = 0; i < 1000; i++)
        {
            argv[i] = malloc(sizeof(char) * 1000);
        }
        argv = gettokens(buffer);
        if (strcmp(argv[0], "READ") == 0)
        {
            // int connecting=connect(sock)
            printf("anbc\n");
            if (recv(sock, ss_ip, 1000, 0) < 0)
            {
                perror("Receive error");
                exit(1);
            }
            if (recv(sock, &ss_port, sizeof(int), 0) < 0)
            {
                perror("Receive error");
                exit(1);
            }

            // ss_ip[7]='\0';
            printf("Received Storage Server IP: %s, Port: %d\n", ss_ip, ss_port);

            // Close the socket to the naming server
            // close(sock);

            // Establish connection with the Storage Server
            struct sockaddr_in ss_server_addr;

            ss_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (ss_sock < 0)
            {
                perror("SS Socket error");
                exit(1);
            }

            memset(&ss_server_addr, '\0', sizeof(ss_server_addr));
            ss_server_addr.sin_family = AF_INET;
            ss_server_addr.sin_port = htons(ss_port);
            ss_server_addr.sin_addr.s_addr = inet_addr(ss_ip);

            while (connect(ss_sock, (struct sockaddr *)&ss_server_addr, sizeof(ss_server_addr)) < 0)
            {
            }
            printf("Connected to the Storage Server.\n");

            // Receive data from the Storage Server until STOP signal or specific condition
            char ss_data[MAX_INPUT_LENGTH];
            while (1)
            {
                ssize_t bytes_received = recv(ss_sock, ss_data, sizeof(ss_data), 0);
                if (bytes_received < 0)
                {
                    perror("Receive error");
                    exit(1);
                }
                else if (bytes_received == 0)
                {
                    printf("Connection closed by Storage Server.\n");
                    break;
                }
                else
                {
                    ss_data[bytes_received] = '\0';
                    if (strcmp(ss_data, STOP_SIGNAL) == 0)
                    {
                        printf("Received STOP signal. Task completed.\n");
                        break;
                    }
                    printf("Received data from Storage Server: %s\n", ss_data);
                }
            }
            close(sock);
            close(ss_sock);
        }
        if (strcmp(argv[0], "WRITE") == 0)
        {
            printf("anbc\n");
            if (recv(sock, ss_ip, 1000, 0) < 0)
            {
                perror("Receive error");
                exit(1);
            }
            if (recv(sock, &ss_port, sizeof(int), 0) < 0)
            {
                perror("Receive error");
                exit(1);
            }

            printf("Received Storage Server IP: %s, Port: %d\n", ss_ip, ss_port);

            struct sockaddr_in ss_server_addr;

            ss_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (ss_sock < 0)
            {
                perror("SS Socket error");
                exit(1);
            }

            memset(&ss_server_addr, '\0', sizeof(ss_server_addr));
            ss_server_addr.sin_family = AF_INET;
            ss_server_addr.sin_port = htons(ss_port);
            ss_server_addr.sin_addr.s_addr = inet_addr(ss_ip);

            while (connect(ss_sock, (struct sockaddr *)&ss_server_addr, sizeof(ss_server_addr)) < 0)
            {
            }
            printf("Connected to the Storage Server.\n");
            char ack_buffer[1024];
            if (recv(sock, ack_buffer, sizeof(ack_buffer), 0) < 0)
            {
                perror("Receive error");
                exit(1);
            }
            printf("recieved ack from nm: %s\n", ack_buffer);
            close(sock);
            close(ss_sock);
        }
        if (strcmp(argv[0], "GET_DETAILS") == 0)
        {
            // int connecting=connect(sock)
            printf("anbc\n");
            if (recv(sock, ss_ip, 1000, 0) < 0)
            {
                perror("Receive error");
                exit(1);
            }
            if (recv(sock, &ss_port, sizeof(int), 0) < 0)
            {
                perror("Receive error");
                exit(1);
            }

            // ss_ip[7]='\0';
            printf("Received Storage Server IP: %s, Port: %d\n", ss_ip, ss_port);

            // Close the socket to the naming server
            // close(sock);

            // Establish connection with the Storage Server
            struct sockaddr_in ss_server_addr;

            ss_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (ss_sock < 0)
            {
                perror("SS Socket error");
                exit(1);
            }

            memset(&ss_server_addr, '\0', sizeof(ss_server_addr));
            ss_server_addr.sin_family = AF_INET;
            ss_server_addr.sin_port = htons(ss_port);
            ss_server_addr.sin_addr.s_addr = inet_addr(ss_ip);

            while (connect(ss_sock, (struct sockaddr *)&ss_server_addr, sizeof(ss_server_addr)) < 0)
            {
            }
            printf("Connected to the Storage Server.\n");
            file_prop *properties = malloc(sizeof(file_prop));
            recv(ss_sock, properties, sizeof(file_prop), 0);
            char file_ack[1024];
            // int req = recv(sock, file_ack, sizeof(file_ack), 0);
            // if (req == -1)
            // {
            //     printf("some error ocurred\n");
            // }
            // printf("ack from nm: %s\n", file_ack);
            if (properties == NULL)
            {
                printf("Error: Memory allocation failed\n");
                exit(1);
            }

            printf("hello..\n");
            printf("file size: %ld %o\n", properties->file_size, properties->permissions);
            close(sock);
            close(ss_sock);
        }
    }
    return 0;
}
