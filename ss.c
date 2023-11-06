#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
typedef struct details{
    char ip[100];
    int port_no;
    char list_of_accesible_paths[200];
    int client_port_no;
}details;
int main() {


  int port = 5000;

  int sock;
  struct sockaddr_in addr;
  socklen_t addr_size;
  char buffer[1024];
  details nm_details;
  memset(&nm_details, '\0', sizeof(struct details));
  strcpy(nm_details.ip,"10.2.133.160");
//   nm_details.ip= ip;
  nm_details.client_port_no= 2000;
  strcpy(nm_details.list_of_accesible_paths,"LIST");
//   nm_details.list_of_accesible_paths="LIST";
  nm_details.port_no=port;
  int n;

  // Error handling for socket() call
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  // Error handling for connect() call
  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr("10.2.133.160");

  n = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
  if (n < 0) {
    perror("Connect error");
    exit(1);
  }
  printf("Connected to the naming server.\n");

if( send(sock, (void*)&nm_details, sizeof(nm_details),0) < 0 ) {
    printf("send failed!\n");
}
    perror("Send error");

bzero(buffer, 1024);
n = recv(sock, buffer, sizeof(buffer), 0);
if (n < 0) {
    perror("Recv error");
    exit(1);
}
printf("%s\n",buffer);
char* var;
char **tokens=(char**)malloc(sizeof(char*)*3);
for (int i = 0; i < 3; i++)
{
     tokens[i]=(char*)malloc(sizeof(char)*100);
}

var = strtok(buffer, " ");
int i=0;
while (var != NULL) {
    // printf("Token: %s\n", token);
    strcpy(tokens[i],var);
    var = strtok(NULL, " ");
    i++;
}
 char command[100];
    snprintf(command, sizeof(command), "mkdir %s%s", tokens[2], tokens[1]);

    // Execute the shell command to create the file
    int result = system(command);

    if (result == 0) {
        printf("File created successfully at: %s%s\n", tokens[2], tokens[1]);
    } else {
        printf("File creation failed.\n");
    }
close(sock);
printf("Disconnected from the server.\n");

  return 0;
}