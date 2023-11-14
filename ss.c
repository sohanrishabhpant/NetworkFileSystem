#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct details
{
  char ip[100];
  int port_no;
  char list_of_accesible_paths[200];
  int client_port_no;
} details;

void create_func(char **argv2,details nm_details){
 if(strcmp(argv2[3],nm_details.list_of_accesible_paths)==0){

      printf("helloc\n");
      if (strcmp(argv2[1],"-d")==0){
        char *kris;
        kris=malloc(sizeof(char)*1000);
        snprintf(kris, 1000, "%s/%s", argv2[3], argv2[2]);
        int stat=mkdir(kris,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(stat==0){
          printf("mkdir is succesfully updated");
        }
      }
      if(strcmp(argv2[1],"-f")==0){
        char *kris;
        kris=malloc(sizeof(char)*1000);
        snprintf(kris, 1000, "%s/%s", argv2[3], argv2[2]);
        int ris=open(kris,O_RDWR | O_CREAT ,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(ris!=-1){
          printf("file created succesfully\n");
        }

      }
    }
}
void connect_ss(char **argv, int port)
{
  int sock;
  char buffer[1024];
  struct sockaddr_in addr;
  socklen_t addr_size;
  details nm_details;
  memset(&nm_details, '\0', sizeof(struct details));
  strcpy(nm_details.ip, "10.2.131.73");
  nm_details.client_port_no = 2000;
  strcpy(nm_details.list_of_accesible_paths, "./rishabh");
  nm_details.port_no = atoi(argv[1]);
  int n;

  // Error handling for socket() call
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  // Error handling for connect() call
  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  n = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (n < 0)
  {
    perror("Connect error");
    exit(1);
  }
  printf("Connected to the naming server.\n");

  if (send(sock, (void *)&nm_details, sizeof(nm_details), 0) < 0)
  {
    printf("send failed!\n");
  }
  perror("Send error");
  // close(sock);
  bzero(buffer, 1024);
  // n = recv(sock, buffer, sizeof(buffer), 0);
  if (n < 0)
  {
    perror("Recv error");
    exit(1);
  }
  char **argv2 = malloc(sizeof(char *) * 1000);
  for (int i = 0; i < 1000; i++)
  {
    argv2[i] = malloc(sizeof(char) * 1000);
  }
  // argv2 = gettokens(buffer);
  n = recv(sock, argv2,, 0);
  if (strcmp(argv2[0],"create")==0){
    printf("hello\n");
    argv2[3][strlen(argv2[3])-1]='\0';
    // printf("%s %s",argv2[3],nm_details.list_of_accesible_paths);
    create_func(argv2,nm_details);
  }
}
int main(int argc, char **argv)
{
  int port = 5000;
  connect_ss(argv, port);
  return 0;
}