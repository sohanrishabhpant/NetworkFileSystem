#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int client_port;
typedef char *string;
typedef struct file_prop
{
  mode_t permissions;
  int file_size;
} file_prop;

typedef struct details
{
  char ip[100];
  int port_no;
  string list_of_accesible_paths[200];
  int client_port_no;
} details;
string *paths;
typedef struct tokenised
{
  string *buffer;
} tokenised;
void create_func(char **argv2)
{
  printf("helloc\n");
  if (strcmp(argv2[1], "-d") == 0)
  {
    char *kris;
    kris = malloc(sizeof(char) * 1000);
    snprintf(kris, 1000, "%s/%s", argv2[3], argv2[2]);
    int stat = mkdir(kris, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (stat == 0)
    {
      printf("mkdi;r is succesfully updated");
    }
  }
  if (strcmp(argv2[1], "-f") == 0)
  {
    char *kris;
    kris = malloc(sizeof(char) * 1000);
    snprintf(kris, 1000, "%s/%s", argv2[3], argv2[2]);
    int ris = open(kris, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (ris != -1)
    {
      printf("file created succesfully\n");
    }
  }
}
void delete_func(char **argv2)
{
  char *kris;
  kris = malloc(sizeof(char) * 1000);
  snprintf(kris, 1000, "%s/%s", argv2[3], argv2[2]);
  if (remove(kris) == 0)
  {
    printf("is deleted\n");
  }
  else
  {
    perror("there is an error\n");
    exit(1);
  }
}

void init_paths(string **paths)
{
  *paths = (string *)malloc(sizeof(string) * 200);
  for (int i = 0; i < 200; i++)
  {
    (*paths)[i] = NULL;
    (*paths)[i] = (string)malloc(sizeof(char) * 1000);
  }
}
void listFiles(const char *path)
{

  DIR *dir;
  struct dirent *entry;

  if ((dir = opendir(path)) == NULL)
  {
    perror("opendir");
    exit(1);
  }
  int i = 0;
  while ((entry = readdir(dir)) != NULL)
  {
    // printf("%s\n", entry->d_name);
    strcpy(paths[i], entry->d_name);
    paths[i][strlen(paths[i]) - 1] = '\0';
    i++;
  }
  closedir(dir);
}

void connect_ss(int port1, int port2, int port)
{
  int sock;
  char buffer[1024];
  struct sockaddr_in addr;
  // socklen_t addr_size;
  details nm_details;
  memset(&nm_details, '\0', sizeof(struct details));
  strcpy(nm_details.ip, "127.0.0.1");
  nm_details.client_port_no = 2000;
  char dir_path[FILENAME_MAX];
  if (fgets(dir_path, sizeof(dir_path), stdin) != NULL)
  {
    if (dir_path[strlen(dir_path) - 1] == '\n')
    {
      dir_path[strlen(dir_path) - 1] = '\0';
    }
  }
  init_paths(&paths);
  // paths[0]="sohan";
  // printf("%s\n",paths[0]);
  listFiles(dir_path);
  int i = 0;

  while ((paths)[i] != NULL)
  {
    nm_details.list_of_accesible_paths[i] = malloc(strlen((paths)[i]) + 1);
    strcpy(nm_details.list_of_accesible_paths[i], (paths)[i]);
    i++;
  }
  nm_details.port_no = port1;
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
  addr.sin_port = htons(port);
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
}
void read_func(string *buffer, int soc)
{
  const char *filePath = buffer[1];
  // Open the file for reading
  int fileDescriptor = open(filePath, O_RDONLY);
  if (fileDescriptor == -1)
  {
    perror("Error opening file");
    exit(1);
  }

  // Determine the size of the file
  off_t fileSize = lseek(fileDescriptor, 0, SEEK_END);
  lseek(fileDescriptor, 0, SEEK_SET);

  // Allocate memory to store the file content
  char *jin = (char *)malloc(fileSize + 1);
  if (jin == NULL)
  {
    perror("Memory allocation error");
    close(fileDescriptor);
    exit(1);
  }

  // Read the entire file into the buffer
  ssize_t bytesRead = read(fileDescriptor, jin, fileSize);
  if (bytesRead != fileSize)
  {
    perror("Error reading file");
    free(jin);
    close(fileDescriptor);
    exit(1);
  }

  // Null-terminate the buffer
  jin[fileSize] = '\0';
  free(jin);
  close(fileDescriptor);

  struct sockaddr_in client;
  int socket1 = socket(AF_INET, SOCK_STREAM, 0);
  if (socket1 == -1)
  {
    perror("socket creation");
    exit(1);
  }
  char *ip = "127.0.0.1";
  struct sockaddr_in cli;
  client.sin_family = AF_INET;
  client.sin_port = htons(client_port);
  client.sin_addr.s_addr = inet_addr(ip);
  int accepting = accept(soc, (struct sockaddr *)&cli, (socklen_t *)sizeof(cli));
  if (accepting < 0)
  {
    perror("Connect error");
    exit(1);
  }
  if (send(soc, jin, sizeof(jin), 0) < 0)
  {
    printf("send failed!\n");
  }
  perror("Send error");
  strcpy(jin, "STOP");
  send(soc, jin, sizeof(jin), 0);
}

void copy_func(string *buffer)
{
  string source = buffer[1];
  string destination = buffer[2];
  struct stat stat_source;
  stat(source, &stat_source);

  if (S_ISDIR(stat_source.st_mode)) {
    // If the source is a directory, create the corresponding directory in the destination
    mkdir(destination, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    DIR *dir = opendir(source);
    if (dir == NULL) {
      perror("Error opening source directory");
      exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }

      char source_child[1024];
      char destination_child[1024];
      snprintf(source_child, 1024, "%s/%s", source, entry->d_name);
      snprintf(destination_child, 1024, "%s/%s", destination, entry->d_name);

      copyFileOrDirectory(source_child, destination_child);
    }

    closedir(dir);
  } else {
    // If the source is a file, copy it to the destination
    int source_file = open(source, O_RDONLY);
    if (source_file == -1) {
      perror("Error opening source file");
      exit(1);
    }

    int destination_file = open(destination, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (destination_file == -1) {
      perror("Error opening destination file");
      exit(1);
    }

    char buf[BUFSIZ];
    ssize_t bytes_read;
    while ((bytes_read = read(source_file, buf, BUFSIZ)) > 0) {
      if (write(destination_file, buf, bytes_read) != bytes_read) {
        perror("Write error");
        exit(1);
      }
    }

    if (bytes_read == -1) {
      perror("Read error");
      exit(1);
    }

    if (close(source_file) == -1 || close(destination_file) == -1) {
      perror("Close error");
      exit(1);
    }
  }
}
void write_func(string *buffer, int soc)
{
  char *ip = "127.0.0.1";
  struct sockaddr_in client;
  client.sin_family = AF_INET;
  client.sin_port = htons(client_port);
  client.sin_addr.s_addr = inet_addr(ip);
  int accepting = accept(soc, (struct sockaddr *)&client, (socklen_t *)sizeof(client));
  if (accepting < 0)
  {
    perror("Connect error");
    exit(1);
  }
  // if (recv(accepting, buffer, sizeof(buffer), 0) < 0)
  // {
  //   printf("recieve failed!\n");
  // }
  // perror("recieve error");
  string text_to_write = buffer[2];

  const char *filePath = buffer[1];

  // Open the file for appending
  int fileDescriptor = open(filePath, O_WRONLY | O_APPEND);
  if (fileDescriptor == -1)
  {
    perror("Error opening file");
    exit(1);
  }

  // Write the buffer text to the file
  size_t bufferLength = strlen(text_to_write);
  ssize_t bytesWritten = write(fileDescriptor, text_to_write, bufferLength);
  if (bytesWritten != bufferLength)
  {
    perror("Error writing to file");
    close(fileDescriptor);
    exit(1);
  }
  close(fileDescriptor);

  printf("Text appended to file successfully.\n");
}
void get_details_func(string *buffer, int soc)
{
  file_prop *properties;
  properties = (file_prop *)malloc(sizeof(properties));
  const char *filePath = buffer[1];
  struct stat stat_buf;
  if (stat(filePath, &stat_buf) == 0)
  {
    // printf("File permissions: %o\n", stat_buf.st_mode);
    // printf("File size: %ld bytes\n", stat_buf.st_size);
    properties->permissions = stat_buf.st_mode;
    properties->file_size = stat_buf.st_size;
  }
  else
  {
    printf("Error retrieving file details\n");
  }
  struct sockaddr_in client;
  char *ip = "127.0.0.1";
  struct sockaddr_in cli;
  client.sin_family = AF_INET;
  client.sin_port = htons(client_port);
  client.sin_addr.s_addr = inet_addr(ip);
  int accepting = accept(soc, (struct sockaddr *)&cli, (socklen_t *)sizeof(cli));
  if (accepting < 0)
  {
    perror("Connect error");
    exit(1);
  }
}
void reciving(int port1)
{
  char *ip = "127.0.0.1";
  int socket1 = socket(AF_INET, SOCK_STREAM, 0);
  if (socket1 == -1)
  {
    perror("socket creation");
    exit(1);
  }
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(port1);
  server.sin_addr.s_addr = inet_addr(ip);
  int binding = bind(socket1, (struct sockaddr *)&server, sizeof(server));
  if (binding < 0)
  {
    perror("bind problem\n");
    exit(3);
  }
  int listening = listen(socket1, 1000);
  struct sockaddr_in nm;
  tokenised token;
  int accepting = accept(socket1, (struct sockaddr *)&nm, (socklen_t *)sizeof(nm));
  int reciving = recv(accepting, &token, sizeof(token), 0);
  if (strcmp(token.buffer[0], "CREATE") == 0)
  {
    create_func(token.buffer);
  }
  if (strcmp(token.buffer[0], "DELETE") == 0)
  {
    delete_func(token.buffer);
  }
  if (strcmp(token.buffer[0], "COPY") == 0)
  {
    copy_func(token.buffer);
  }
  if (strcmp(token.buffer[0], "READ") == 0)
  {
    read_func(token.buffer, socket1);
  }
  if (strcmp(token.buffer[0], "WRITE") == 0)
  {
    write_func(token.buffer, socket1);
  }
  if (strcmp(token.buffer[0], "GET_DETAILS") == 0)
  {
    get_details_func(token.buffer, socket1);
  }
  if (strcmp(token.buffer[0], "COPY") == 0)
  {
    copy_func(token.buffer);
  }
}
int main(int argc, char **argv)
{
  int port = 5000;

  // client_port= 2000;
  connect_ss(atoi(argv[1]), atoi(argv[2]), port);
  // connect_ss(3000,2000, port);
  client_port = atoi(argv[2]);
  reciving(atoi(argv[1]));
  // reciving(3000);

  free(paths);
  return 0;
}