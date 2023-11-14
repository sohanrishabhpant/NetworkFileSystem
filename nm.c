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
details ss_dets[100];
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
int main() {

  char *ip = "127.0.0.1";
  int port = 5000;

  int server_sock, ss_sock;
  struct sockaddr_in server_addr, ss_addr;
  socklen_t addr_size;
  char buffer[1024];
  int n;

  // Error handling for socket() call
  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0) {
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  // Error handling for bind() call
  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (n < 0) {
    perror("Bind error");
    exit(1);
  }
  printf("Bind to the port number: %d\n", port);

  // Error handling for listen() call
  listen(server_sock, 5);
  details nm_details;
  memset(&nm_details, '\0', sizeof(struct details));
    
    int ss_count=0;
  while (1) {
    addr_size = sizeof(ss_addr);
    ss_sock = accept(server_sock, (struct sockaddr*)&ss_addr, &addr_size);
    printf("ss %d connected.\n",ss_count);
    memset(&ss_dets[ss_count],'\0',sizeof(struct details));
    
    // Error handling for recv() call
    bzero(buffer, 1024);
    recv(ss_sock, &ss_dets[ss_count], sizeof(details), 0);
    printf("ss %d ip: %s\n",ss_count,ss_dets[ss_count].ip);
    printf("ss %d port_no: %d\n",ss_count,ss_dets[ss_count].port_no);
    printf("ss %d list_of_paths: %s\n",ss_count,ss_dets[ss_count].list_of_accesible_paths);
    printf("ss %d client_port_no: %d\n",ss_count,ss_dets[ss_count].client_port_no);
    // strcpy(buffer,"CREATE file test/");
    fgets(buffer,1000,stdin);
    char **argv2 = malloc(sizeof(char *) * 1000);
  for (int i = 0; i < 1000; i++)
  {
    argv2[i] = malloc(sizeof(char) * 1000);
  }
  argv2=gettokens(buffer);
    n = send(ss_sock, argv2, strlen(buffer), 0);
    if (n < 0) {
        perror("Send error");
        exit(1);
    }
    printf("data sent\n");
    ss_count++;
  }
    close(ss_sock);
    printf("[+]Client disconnected.\n\n");
    
  return 0;
}