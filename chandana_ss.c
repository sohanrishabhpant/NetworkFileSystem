#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef char* string;

typedef struct details{
    char ip[100];
    int port_no;
    string list_of_accesible_paths[200];
    int client_port_no;
}details;
string *paths;
void create_func(char **argv2){
 

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
void delete_func(char **argv2){
    char *kris;
        kris=malloc(sizeof(char)*1000);
        snprintf(kris, 1000, "%s/%s", argv2[3], argv2[2]);
        if (remove(kris)==0){
          printf("is deleted\n");
        }
        else{
          perror("there is an error\n");
          exit(1);
        }
}

void init_paths(string **paths){
  *paths=(string*)malloc(sizeof(string)*200);
  for (int i = 0; i < 200; i++)
  {
   (*paths)[i]=NULL;
   (*paths)[i]=(string)malloc(sizeof(char)*1000);
  }
  
}
void listFiles(const char *path) {

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        // printf("%s\n", entry->d_name);
        strcpy(paths[i], entry->d_name);
        paths[i][strlen(paths[i])-1]='\0';
        i++;
    }
    closedir(dir);
}

void connect_ss(int port1,int port){
  int sock;
  char buffer[1024];
  struct sockaddr_in addr;
  socklen_t addr_size;
  details nm_details;
  memset(&nm_details, '\0', sizeof(struct details));
  strcpy(nm_details.ip,"127.0.0.1");
  nm_details.client_port_no= 2000;
  char dir_path[FILENAME_MAX];
    if (fgets(dir_path, sizeof(dir_path), stdin) != NULL) {
        if (dir_path[strlen(dir_path) - 1] == '\n') {
            dir_path[strlen(dir_path) - 1] = '\0';
        }
    }  
  init_paths(&paths);
  // paths[0]="sohan";
  // printf("%s\n",paths[0]);
  listFiles(dir_path);
  int i=0;

  while ((paths)[i]!=NULL)
  {
    nm_details.list_of_accesible_paths[i] = malloc(strlen((paths)[i]) + 1);
      strcpy(nm_details.list_of_accesible_paths[i],(paths)[i]);
      i++;
  }
  nm_details.port_no=port1;
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
  addr.sin_port =htons(port);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

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
  // close(sock);
  bzero(buffer, 1024);
}
// int main(int argc,char**argv) {
//   int port = 5000;
//   connect_ss(argv[2],port);
//   free(paths);
//   return 0;
// }
int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s <storage_server_port> <naming_server_port>\n", argv[0]);
        return 1;
    }

    int storage_server_port = atoi(argv[1]);
    int naming_server_port = atoi(argv[2]);

    // init_paths();

    char dir_path[MAX_PATH_LENGTH];
    printf("Enter directory path: ");
    if (fgets(dir_path, sizeof(dir_path), stdin) != NULL) {
        if (dir_path[strlen(dir_path) - 1] == '\n') {
            dir_path[strlen(dir_path) - 1] = '\0';
        }
    }

    listFiles(dir_path);

    details server_details;
    strcpy(server_details.ip, "127.0.0.1");
    server_details.client_port_no = CLIENT_PORT;

    int i = 0;
    while (paths[i] != NULL && i < MAX_ACCESSIBLE_PATHS) {
        server_details.list_of_accessible_paths[i] = malloc(strlen(paths[i]) + 1);
        strcpy(server_details.list_of_accessible_paths[i], paths[i]);
        i++;
    }
    server_details.port_no = storage_server_port;

    connect_nm(storage_server_port, naming_server_port);

    free(paths);

    return 0;
}