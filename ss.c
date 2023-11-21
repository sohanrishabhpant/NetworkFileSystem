#include "trie.c"
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

int client_port;
int ss_port;
typedef char *string;
sem_t mutex;
typedef struct file_prop
{
  mode_t permissions;
  size_t file_size;
} file_prop;
typedef struct for_lock{
  char list_of_accesible_paths[100];
  int index;
  sem_t mutex;
  int flag;
}lock;
typedef struct details
{
  char ip[15];
  int port_no;
  char list_of_accesible_paths[2000];
  int client_port_no;
} details;
string *paths;
typedef struct tokenised
{
  string *buffer;
} tokenised;
details nm_details;
typedef struct chunks
{
  int sequence_num;
  char string[1024];
} chunk;
lock var1[100];
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
void create_func(char **argv2, int socket1)
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
      printf("mkdir is succesfully updated");
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
      char create_ack[1024];
      if (send(socket1, create_ack, sizeof(create_ack), 0) < 0)
      {
        perror("error in sending ack to nm");
      }
    }
  }
}
void delete_func(char **argv2, int socket1)
{
  char *kris;
  kris = malloc(sizeof(char) * 1000);
  snprintf(kris, 1000, "%s/%s", argv2[2], argv2[1]);
  if (remove(kris) == 0)
  {
    printf("is deleted\n");
    char delete_ack[1024];
    if (send(socket1, delete_ack, sizeof(delete_ack), 0) < 0)
    {
      perror("error in sending ack to nm");
    }
  }
  else
  {
    perror("there is an error\n");
    exit(1);
  }
}
void read_func(char **argv2, int soc)
{
  char *kris;
  kris = malloc(sizeof(char) * 1000);
  snprintf(kris, 1000, "%s/%s", argv2[2], argv2[1]);
  int fileDescriptor = open(kris, O_RDONLY);
  if (fileDescriptor == -1)
  {
    perror("Error opening file");
    exit(1);
  }
  printf("hello3\n");
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

  // Read the file in chunks
  ssize_t bytesRead;
  size_t totalRead = 0;
  while ((bytesRead = read(fileDescriptor, jin + totalRead, fileSize - totalRead)) > 0)
  {
    totalRead += bytesRead;
  }

  if (bytesRead < 0)
  {
    perror("Error reading file");
    free(jin);
    close(fileDescriptor);
    exit(1);
  }

  // Null-terminate the buffer
  jin[totalRead] = '\0';

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
  int binding = bind(socket1, (struct sockaddr *)&client, sizeof(client));
  if (binding < 0)
  {
    perror("binding\n");
    exit(2);
  }
  int listening = listen(socket1, 10);
  socklen_t var = sizeof(cli);
  printf("hello1\n");
  // while(1){
  int accepting = accept(socket1, (struct sockaddr *)&cli, &var);
  if (accepting < 0)
  {
    perror("Connect error");
    exit(1);
  }
  int n;
  printf("anc\n");
  n = send(accepting, jin, fileSize + 1, 0);
  printf("n:%d\n", n);
  if (n < 0)
  {
    printf("send failed!\n");
  }
  perror("Send error");
  strcpy(jin, "STOP");
  send(accepting, jin, sizeof(jin), 0);
  update_log(ss_port,1);
}

void copyFileOrDirectory(string source, string destination)
{
  struct stat stat_source;
  stat(source, &stat_source);

  if (S_ISDIR(stat_source.st_mode))
  {
    // If the source is a directory, create the corresponding directory in the destination
    mkdir(destination, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    DIR *dir = opendir(source);
    if (dir == NULL)
    {
      perror("Error opening source directory");
      exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      {
        continue;
      }

      char source_child[1024];
      char destination_child[1024];
      snprintf(source_child, 1024, "%s/%s", source, entry->d_name);
      snprintf(destination_child, 1024, "%s/%s", destination, entry->d_name);

      copyFileOrDirectory(source_child, destination_child);
    }

    closedir(dir);
  }
  else
  {
    // If the source is a file, copy it to the destination
    int source_file = open(source, O_RDONLY);
    if (source_file == -1)
    {
      perror("Error opening source file");
      exit(1);
    }

    int destination_file = open(destination, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (destination_file == -1)
    {
      perror("Error opening destination file");
      exit(1);
    }

    char buf[BUFSIZ];
    ssize_t bytes_read;
    while ((bytes_read = read(source_file, buf, BUFSIZ)) > 0)
    {
      if (write(destination_file, buf, bytes_read) != bytes_read)
      {
        perror("Write error");
        exit(1);
      }
    }

    if (bytes_read == -1)
    {
      perror("Read error");
      exit(1);
    }

    if (close(source_file) == -1 || close(destination_file) == -1)
    {
      perror("Close error");
      exit(1);
    }
  }
}

void copy_func(string *buffer, int socket1)
{
  printf("intit\n");
  string source = buffer[1];
  string destination = buffer[2];
  struct stat stat_source;
  stat(source, &stat_source);

  if (S_ISDIR(stat_source.st_mode))
  {
    // If the source is a directory, create the corresponding directory in the destination
    mkdir(destination, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    DIR *dir = opendir(source);
    if (dir == NULL)
    {
      perror("Error opening source directory");
      exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      {
        continue;
      }

      char source_child[1024];
      char destination_child[1024];
      snprintf(source_child, 1024, "%s/%s", source, entry->d_name);
      snprintf(destination_child, 1024, "%s/%s", destination, entry->d_name);

      copyFileOrDirectory(source_child, destination_child);
    }

    closedir(dir);
  }
  else
  {
    printf("inting\n");
    printf("sour:%s\n",source);
    int source_file = open(source, O_RDONLY);
    if (source_file == -1)
    {
      perror("Error opening source file");
      exit(1);
    }
    char buf[BUFSIZ];
    int chunksize = BUFSIZ / 1000;
    int count = 1;
    ssize_t bytes_read;
    char **temp = malloc(sizeof(char *) * 1000);
    for (int i = 0; i < 1000; i++)
    {
      temp[i] = malloc(sizeof(char) * 1000);
    }
    details nm_details1;
    nm_details1 = nm_details;
    temp = gettokens(nm_details1.list_of_accesible_paths);
    int sear = 0;
    int temp_index = 0;
    while (temp[temp_index])
    {
      if (strcmp(temp[temp_index], source) == 0)
      {
        sear = 1;
        break;
      }
      temp_index++;
    }
    printf("sear:%d\n",sear);
    if (sear == 1)
    {
      while ((bytes_read = read(source_file, &buf[count * 1000], 1000)) > 0 && chunksize < 1000)
      {
        printf("%ld\n", bytes_read);
        if (send(socket1, &buf[count * 1000], 1000, 0) < 0)
        {
          perror("send error");
        }
      }
      if (bytes_read == -1)
      {
        perror("Read error");
        exit(1);
      }

      if (close(source_file) == -1)
      {
        perror("Close error");
        exit(1);
      }
    }
    else
    {
      char buffer5[BUFSIZ];
      int chunksize=BUFSIZ/1000;
      count = 1;
      char *str=malloc(sizeof (char)*1000);
      str="";
      printf("risha\n");
        int opening=open(destination,O_RDWR);
      while (count < chunksize)
      {
        recv(socket1, &buffer5[count * 1000], 1000, 0);
        printf("re\n");
      
        write(opening,&buffer5[count*1000],1000);
        count++;
      }
    }
  }
}
void write_func(char **argv2, int sock)
{
  printf("arg: %s\n", argv2[3]);
  char *ip = "127.0.0.1";
  int socket1 = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in client;
  client.sin_family = AF_INET;
  client.sin_port = htons(client_port);
  client.sin_addr.s_addr = inet_addr(ip);
  int binding = bind(socket1, (struct sockaddr *)&client, sizeof(client));
  struct sockaddr_in cli;
  int listening = listen(socket1, 10);
  socklen_t var = sizeof(cli);
  int accepting = accept(socket1, (struct sockaddr *)&cli, &var);
  if (accepting < 0)
  {
    perror("Connect error");
    exit(1);
  }

  string text_to_write = argv2[3];

  char *kris;
  kris = malloc(sizeof(char) * 1000);
  snprintf(kris, 1000, "%s/%s", argv2[2], argv2[1]);

  // Open the file for appending
  int index=0;
  for (int i=0;i<100;i++){
    if (strcmp(var1[i].list_of_accesible_paths,kris)==0){
      index=i;
    }
  }
  sem_wait(&var1[index].mutex);
  int fileDescriptor = open(kris, O_WRONLY | O_APPEND);
  if (fileDescriptor == -1)
  {
    perror("Error opening file");
    exit(1);
  }

  // Write the buffer text to the file
  size_t bufferLength = strlen(text_to_write);
  size_t totalBytesWritten = 0;

  while (totalBytesWritten < bufferLength)
  {
    ssize_t bytesWritten = write(fileDescriptor, text_to_write + totalBytesWritten, bufferLength - totalBytesWritten);

    if (bytesWritten == -1)
    {
      perror("Error writing to file");
      close(fileDescriptor);
      exit(1);
    }

    totalBytesWritten += bytesWritten;
  }
  sem_post(&var1[index].mutex);
  var1[index].flag=1;
  close(fileDescriptor);

  printf("Text appended to file successfully.\n");
  update_log(ss_port,1);
}
void get_details_func(string *buffer, int sock)
{
  file_prop *properties;
  properties = (file_prop *)malloc(sizeof(properties));
  // const char *filePath = buffer[1];
  char *file_Path;
  file_Path = malloc(sizeof(char) * 1000);
  snprintf(file_Path, 1000, "%s/%s", buffer[2], buffer[1]);

  struct stat stat_buf;
  if (stat(file_Path, &stat_buf) == 0)
  {
    properties->permissions = stat_buf.st_mode;
    // printf("%d..\n",properties->file_size);
    properties->file_size = stat_buf.st_size;
  }
  else
  {
    printf("Error retrieving file details\n");
  }
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
  int binding = bind(socket1, (struct sockaddr *)&client, sizeof(client));
  if (binding < 0)
  {
    perror("binding\n");
    exit(2);
  }
  int listening = listen(socket1, 10);
  socklen_t var = sizeof(cli);
  printf("hello1\n");
  // while(1){
  int accepting = accept(socket1, (struct sockaddr *)&cli, &var);
  if (accepting < 0)
  {
    perror("Connect error");
    exit(1);
  }
  if (send(accepting, properties, sizeof(file_prop), 0) < 0)
  {
    printf("send failed!\n");
  }
  update_log(ss_port,1);
  // char file_ack[1024];
  // strcpy(file_ack, "details fetched succesfully");
  // if (send(accepting, file_ack, sizeof(file_ack), 0) < 0)
  // {
  //   printf("send failed!\n");
  // }
}

void init_paths(string **paths)
{
  *paths = (string *)malloc(sizeof(string) * 200);
  for (int i = 0; i < 20; i++)
  {
    (*paths)[i] = (string)malloc(sizeof(char) * 1000);
  }
}

void connect_ss(int port1, int port)
{
  int sock;
  char buffer[1024];
  struct sockaddr_in addr;
  socklen_t addr_size;
  
  // memset(&nm_details, '\0', sizeof(struct details));
  strcpy(nm_details.ip, "127.0.0.1");
  nm_details.client_port_no = client_port;
  char dir_path[FILENAME_MAX];
  if (fgets(dir_path, sizeof(dir_path), stdin) != NULL)
  {
    if (dir_path[strlen(dir_path) - 1] == '\n')
    {
      dir_path[strlen(dir_path) - 1] = '\0';
    }
  }
  printf("%s\n", dir_path);
  init_paths(&paths);
  int i = 0;
  nm_details.port_no = port1;
  strcpy(nm_details.list_of_accesible_paths, dir_path);
  int n;
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
  int flag = 2;
  if (send(sock, &flag, sizeof(int), 0) < 0)
  {
    perror("sending problem\n");
    exit(1);
  }
  // printf("rishabh:%s\n",nm_details.list_of_accesible_paths);

  if (send(sock, &nm_details, sizeof(details), 0) < 0)
  {
    printf("send failed!\n");
  }
  memset(buffer, '\0', 1000);
  recv(sock, buffer, 1000, 0);
  printf("%s\n", buffer);
  // perror("Send error");
  // close(sock);
  bzero(buffer, 1024);
}
void reciving(int port1)
{
  char *ip = "127.0.0.1";
  printf("oh\n");
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
  token.buffer = malloc(sizeof(char *) * 1000);
  for (int i = 0; i < 1000; i++)
  {
    token.buffer[i] = malloc(sizeof(char) * 1000);
  }
  socklen_t var = sizeof(nm);
  char buffer[1024];
  printf("rishabh\n");
  int accepting;
  while((accepting=accept(socket1, (struct sockaddr *)&nm, &var))<0){}
  int reciving = recv(accepting, &buffer, 1000, 0);
  token.buffer = gettokens(buffer);
  printf("buff:%s\n", token.buffer[0]);
  if (reciving < 0)
  {
    perror("rec_problem\n");
    exit(3);
  }
  if (strcmp(token.buffer[0], "CREATE") == 0)
  {
    create_func(token.buffer, accepting);
  }
  if (strcmp(token.buffer[0], "DELETE") == 0)
  {
    delete_func(token.buffer, accepting);
  }
  if (strcmp(token.buffer[0], "READ") == 0)
  {
    read_func(token.buffer, accepting);
  }
  if (strcmp(token.buffer[0],"COPY")==0){
    copy_func(token.buffer,accepting);
  }
  if (strcmp(token.buffer[0], "WRITE") == 0)
  {
    write_func(token.buffer, accepting);
  }
  if (strcmp(token.buffer[0], "GET_DETAILS") == 0)
  {
    get_details_func(token.buffer, accepting);
  }
}
int main(int argc, char **argv)
{
    
  details nm_details2=nm_details;
  char **tokens=malloc(sizeof (char*)*1000);
  for (int i=0;i<1000;i++){
    tokens[i]=malloc(sizeof (char)*1000);
  }
  tokens=gettokens(nm_details2.list_of_accesible_paths);
  int ind=0;
  while(tokens[ind]){
    strcpy(var1[ind].list_of_accesible_paths,tokens[ind]);
    ind++;
    sem_init(&var1[ind].mutex,0,1);
    var1[ind].flag=0;
  }
  int port = 5000;
  // sem_init(&mutex, 0, 1);
  client_port=atoi(argv[2]);
  connect_ss(atoi(argv[1]), port);
  reciving(atoi(argv[1]));
  
  // printf("%d..\n",client_port);
  ss_port=atoi(argv[1]);
  free(paths);
  memset(&client_port, '\0', sizeof(client_port));
  return 0;
}