#include "trie.c"
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#define MAX_PATH_SIZE 4096
int server_sock;
socklen_t addr_size;
#define BUFFER_SIZE 1000
typedef char *string;
LRUCache *cache;
typedef struct details
{
  char ip[15];
  int port_no;
  char list_of_accesible_paths[20];
  int client_port_no;
} details;
typedef struct real_details
{
  char ip[15];
  int port_no;
  trie *list_of_accessible_paths;
  int client_port_no;
} real_details;
typedef struct tokenised
{
  string *buffer;
} tokenised;
int client_flag = 0;
string *paths[100];
details ss_dets[100];
real_details flag[100];
int ss_count;
int client_sock[1000];
struct sockaddr_in ss_addr[1000];
struct sockaddr_in client_addr[1000];
int ss_sock[1000];
char **gettokens(char *token)
{
  int count;
  char **argv = malloc(sizeof(char *) * 1000);
  for (int i = 0; i < 1000; i++)
  {
    argv[i] = malloc(sizeof(char) * 1000);
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
char *ip = "127.0.0.1";
int port = 5000;

void handle_client(char *buffer, int sock)
{
  // printf("helllo1\n");
  char **tokens = malloc(sizeof(char *) * 1000);
  for (int i = 0; i < 1000; i++)
  {
    tokens[i] = malloc(sizeof(char) * 1000);
  }
  // printf("eb\n");
  char buffer1[1024];
  char buffer2[1024];
  strcpy(buffer1, buffer);
  strcpy(buffer2, buffer);
  tokens = gettokens(buffer);
  int index;
  // printf("bab\n");
  if (strcmp(tokens[0], "COPY") != 0)
  {
    printf("ej\n");
    string req_path = malloc(sizeof(char) * 1000);
    req_path = tokens[2];
    printf("sab\n");
    int i;

    int ris;
    index = -1;
    // printf("ss:%d\n",ss_count);
    char *kris;
    kris = malloc(sizeof(char) * 1000);
    snprintf(kris, 1000, "%s/%s", tokens[2], tokens[1]);
    char *ans = malloc(sizeof(char) * 1000);
    ans = fetchFromCache(cache, kris);
    if (ans == NULL)
    {
      addToCache(cache, kris, "recived");
      for (int k = 0; k < ss_count; k++)
      {
        // printf("hello");
        int j = 0;
        int var = search(flag[k].list_of_accessible_paths, req_path);
        printf("out:%d\n", var);
        if (var == 1)
        {
          index = k;
          break;
        }
      }
    }
    int ss_port = ss_dets[index].port_no;
    // printf("port:%d\n", ss_port);
    int new_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in new_addr;
    if (new_sock < 0)
    {
      perror("[-]Socket error");
      exit(1);
    }
    printf("[+]TCP server socket created.\n");

    // Error handling for bind() call
    memset(&new_addr, '\0', sizeof(new_addr));
    new_addr.sin_family = AF_INET;
    new_addr.sin_port = htons(ss_port);
    new_addr.sin_addr.s_addr = inet_addr(ip);
    int n1 = connect(new_sock, (struct sockaddr *)&new_addr, sizeof(new_addr));
    if (n1 < 0)
    {
      perror("Connect error");
      exit(1);
    }
    printf("Connected to the naming server.\n");
    tokenised token;
    token.buffer = malloc(sizeof(char *) * 1000);
    for (int i = 0; i < 1000; i++)
    {
      token.buffer[i] = malloc(sizeof(char) * 1000);
    }
    int ind = 0;
    while (tokens[ind])
    {
      strcpy(token.buffer[ind], tokens[ind]);
      ind++;
    }
    printf("str:%s\n", token.buffer[0]);
    if (strcmp(tokens[0], "COPY") != 0)
    {
      if (strcmp(tokens[0], "CREATE") == 0)
      {
        if (index==-1)
        {
          // Send error code for file in use
          ErrorCode error = ERROR_PERMISSION_DENIED;
          send(sock, &error, sizeof(ErrorCode), 0);
          return;
        }
      }
      if(strcmp(tokens[0],"DELETE")==0){
        if (index==-1)
        {
          // Send error code for file in use
          ErrorCode error = ERROR_FILE_NOT_FOUND;
          send(sock, &error, sizeof(ErrorCode), 0);
          return;
        }
      }
      if (send(new_sock, &buffer1, 1000, 0) < 0)
      {
        printf("send failed!\n");
      }
    }

    perror("Send error");
    char ack_msg[1024];
    if (recv(new_sock, ack_msg, sizeof(ack_msg), 0) < 0)
    {
      printf("Recieve failed!\n");
    }
    update_log(ss_dets[ss_count].port_no, 1);
  }

  if (strcmp(tokens[0], "COPY") == 0)
  {
    // if (file_in_use)
    //     {
    //       // Send error code for file in use
    //       ErrorCode error = ERROR_FILE_IN_USE;
    //       send(sock, &error, sizeof(ErrorCode), 0);
    //       return;
    //     }
    printf("hello1\n");
    int index1;
    for (int l = 0; l < ss_count; l++)
    {
      // printf("hello");
      int j = 0;
      int var = search(flag[l].list_of_accessible_paths, tokens[2]);
      printf("out:%d\n", var);
      if (var == 1)
      {
        index1 = l;
        break;
      }
    }
    int index2;

    for (int l = 0; l < ss_count; l++)
    {
      // printf("hello");
      int j = 0;
      int var = search(flag[l].list_of_accessible_paths, tokens[1]);
      printf("out:%d\n", var);
      if (var == 1)
      {
        index2 = l;
        break;
      }
    }
    int chunks = 0;
    int dest_port = ss_dets[index1].port_no;
    int cl_port = ss_dets[index2].port_no;
    int new_sock1 = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in new_addr;
    if (new_sock1 < 0)
    {
      perror("[-]Socket error");
      exit(1);
    }
    int new_sock2 = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client;
    client.sin_port = htons(cl_port);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    client.sin_family = AF_INET;
    int connecting = connect(new_sock2, (struct sockaddr *)&client, sizeof(client));
    int sendig = send(new_sock2, buffer2, sizeof(buffer2), 0);
    printf("[+]TCP server socket created.\n");

    // Error handling for bind() call
    memset(&new_addr, '\0', sizeof(new_addr));
    new_addr.sin_family = AF_INET;
    new_addr.sin_port = htons(dest_port);
    new_addr.sin_addr.s_addr = inet_addr(ip);
    int n1 = connect(new_sock1, (struct sockaddr *)&new_addr, sizeof(new_addr));
    int sending = send(new_sock1, buffer2, sizeof(buffer2), 0);
    if (n1 < 0)
    {
      perror("Connect error");
      exit(1);
    }
    printf("Connected to the naming server.\n");
    char buffer4[BUFFER_SIZE];
    int chunks1 = BUFFER_SIZE / 1000;
    int count = 1;
    int flag = 3;
    printf("dis\n");
    while (count < chunks1)
    {

      int reciving = recv(new_sock2, &buffer4[count * 1000], 1000, 0);
      printf("%s\n", buffer2);
      int sending = send(new_sock1, &buffer4[count * 1000], 1000, 0);
      count++;
    }
  }

  if (strcmp(tokens[0], "READ") == 0 || strcmp(tokens[0], "WRITE") == 0 || strcmp(tokens[0], "GET_DETAILS") == 0)
  {
    if(strcmp(tokens[0], "READ") == 0 || strcmp(tokens[0], "GET_DETAILS") == 0){
      // Check if file is available
    if (index ==-1) {
        // Send error code for file not found
        ErrorCode error = ERROR_FILE_NOT_FOUND;
        send(sock, &error, sizeof(ErrorCode), 0);
        return;
    }

    // if (file_in_use) {
    //     // Send error code for file in use
    //     ErrorCode error = ERROR_FILE_IN_USE;
    //     send(sock, &error, sizeof(ErrorCode), 0);
    //     return;
    // }
    }
    if(strcmp(tokens[0],"WRITE")==0){
    //  if (file_in_use) {
    //     // Send error code for file in use
    //     ErrorCode error = ERROR_FILE_IN_USE;
    //     send(sock, &error, sizeof(ErrorCode), 0);
    //     return;
    // }
    }

    printf("hello\n");
    printf("inde:%d\n", index);
    int ss_port = ss_dets[index].client_port_no;
    // char* ip_add=ss_dets[index].ip;
    char *ip_add = malloc(sizeof(char) * 1000);
    // ip_add=ss_dets[index].ip;
    strcpy(ip_add, ss_dets[index].ip);
    // client_sock[count] = accept(server_sock, (struct sockaddr *)&client_addr[count], &addr_size);
    printf("hello\n");
    printf("string:%s\n", ip_add);
    int n1 = send(sock, ip_add, 1000, 0);
    printf("n:%d\n", n1);
    if (n1 < 0)
    {
      perror("send");
    }
    n1 = send(ss_sock[ss_count], &ss_port, sizeof(ss_port), 0);
    if (n1 < 0)
    {
      perror("send");
    }
    // wait for ack from ss.
  }
}

void init_paths(string **paths1)
{
  *paths1 = (string *)malloc(sizeof(string) * 200);
  for (int i = 0; i < 20; i++)
  {
    (*paths1)[i] = (string)malloc(sizeof(char) * 1000);
  }
}
void inserting(char **argv, int index)
{
  int j = 0;
  flag[index].list_of_accessible_paths = malloc(sizeof(trie));
  while (argv[j] != NULL)
  {
    insert(flag[index].list_of_accessible_paths, argv[j]);
    j++;
  }
}
void *receive_buffer(void *arg)
{
  char buffer[1000];
  int count = 0;
  // addr_size = sizeof(client_addr[count]);
  // client_sock[count] = accept(server_sock, (struct sockaddr *)&client_addr[count], &addr_size);
  bzero(buffer, 1000);
  // printf("jinchiki\n");
  recv(ss_sock[ss_count], buffer, sizeof(buffer), 0);
  char ack[1024];
  bzero(ack, 1024);
  strcpy(ack, "Your request is being processed...");
  if (send(ss_sock[ss_count], ack, sizeof(ack), 0) < 0)
  {
    perror("send");
  }
  // printf("buffer:%s\n", buffer);
  add_log(ss_dets[ss_count].ip, ss_dets[ss_count].port_no, buffer, 0);
  count++;
  handle_client(buffer, ss_sock[ss_count]);
}

void *accept_ss(void *arg)
{
  printf("hr\n");
  char buffer[1000];
  details nm_details;
  // addr_size = sizeof(ss_addr[ss_count]);
  // ss_sock[ss_count] = accept(server_sock, (struct sockaddr *)&ss_addr[ss_count], &addr_size);
  printf("ss %d connected.\n", ss_count);
  recv(ss_sock[ss_count], &nm_details, sizeof(details), 0);
  ss_dets[ss_count] = nm_details;
  strcpy(flag[ss_count].ip, nm_details.ip);
  flag[ss_count].client_port_no = nm_details.client_port_no;
  flag[ss_count].port_no = nm_details.port_no;
  add_log(flag[ss_count].ip, flag[ss_count].port_no, "INTILISATION", 0);
  printf("ss %d ip: %s\n", ss_count, ss_dets[ss_count].ip);
  printf("ss %d port_no: %d\n", ss_count, ss_dets[ss_count].port_no);
  printf("ss %d client_port_no: %d\n", ss_count, ss_dets[ss_count].client_port_no);
  init_paths(&paths[ss_count]);
  char **argv = malloc(sizeof(char *) * 1000);
  for (int i = 0; i < 1000; i++)
  {
    argv[i] = malloc(sizeof(char) * 1000);
  }
  argv = gettokens(nm_details.list_of_accesible_paths);
  inserting(argv, ss_count);
  int ind1 = 0;
  while (argv[ind1] != NULL)
  {
    printf("ss %d list_of_paths: %s\n", ss_count, argv[ind1]);
    ind1++;
  }
  ss_count++;
}

int main()
{
  cache = createCache();
  struct sockaddr_in server_addr;
  char buffer[1024];
  int n;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0)
  {
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");
  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (n < 0)
  {
    perror("Bind error");
    exit(1);
  }
  printf("Bind to the port number: %d\n", port);

  listen(server_sock, 1000);
  details nm_details;
  memset(&nm_details, '\0', sizeof(struct details));

  // int ss_count = 0;
  ss_count = 0;
  int abou = 0;
  while (1)
  {
    abou = 0;
    printf("hr\n");
    addr_size = sizeof(ss_addr[ss_count]);
    ss_sock[ss_count] = accept(server_sock, (struct sockaddr *)&ss_addr[ss_count], &addr_size);
    printf("ss %d connected.\n", ss_count);
    recv(ss_sock[ss_count], &abou, sizeof(int), 0);
    printf("nm:%d\n",nm_details.client_port_no);
    recv(ss_sock[ss_count], &nm_details, sizeof(details), 0);
    ss_dets[ss_count] = nm_details;
    strcpy(flag[ss_count].ip, nm_details.ip);
    flag[ss_count].client_port_no = nm_details.client_port_no;
    flag[ss_count].port_no = nm_details.port_no;
    add_log(flag[ss_count].ip, flag[ss_count].port_no, "INITLISATION", 0);
    printf("ss %d ip: %s\n", ss_count, ss_dets[ss_count].ip);
    printf("ss %d port_no: %d\n", ss_count, ss_dets[ss_count].port_no);
    printf("ss %d client_port_no: %d\n", ss_count, ss_dets[ss_count].client_port_no);
    init_paths(&paths[ss_count]);
    char **argv = malloc(sizeof(char *) * 1000);
    for (int i = 0; i < 1000; i++)
    {
      argv[i] = malloc(sizeof(char) * 1000);
    }
    argv = gettokens(nm_details.list_of_accesible_paths);
    inserting(argv, ss_count);
    int ind1 = 0;
    while (argv[ind1] != NULL)
    {
      printf("ss %d list_of_paths: %s\n", ss_count, argv[ind1]);
      ind1++;
    }
    memset(buffer, '\0', 1000);
    // fgets(buffer,1000,stdin);
    // scanf("%s",buffer);
    strcpy(buffer, "ack:storage server is connected to nm.");
    // printf("buffer:%s\n",buffer);
    n = send(ss_sock[ss_count], buffer, 1000, 0);
    if (n < 0)
    {
      perror("Send error");
      exit(1);
    }
    // printf("data sent\n");
    close(ss_sock[ss_count]);
    ss_count++;
    char buffer_temp[1000];
    memset(buffer_temp, '\0', 1000);
    fgets(buffer_temp, 1000, stdin);
    printf("%s", buffer_temp);
    if (strncmp(buffer_temp, "done", 4) == 0)
    {
      printf("good\n");
      break;
    }
  }
  int flag = 0;
  int flag1 = 0;
  pthread_t thread1, thread2;
  while (1)
  {
    flag = 0;
    addr_size = sizeof(ss_addr[ss_count]);
    ss_sock[ss_count] = accept(server_sock, (struct sockaddr *)&ss_sock[ss_count], &addr_size);
    // int reciving=recv(ss_sock[ss_count],&flag,sizeof(int),0);
    int reciving = recv(ss_sock[ss_count], &flag1, sizeof(int), 0);
    if (flag1 == 2)
    {

      // printf("nee_abbda\n");
      printf("%d\n", ss_count);
      if (pthread_create(&thread1, NULL, &accept_ss, NULL))
      {
        perror("thread_creation\n");
        exit(1);
      }
      pthread_join(thread1, NULL);
    }

    if (flag1 == 1)
    {
      if (pthread_create(&thread2, NULL, &receive_buffer, NULL))
      {
        perror("thread_creation\n");
        exit(2);
      }
      pthread_join(thread2, NULL);
    }
    display_logs();
  }
  printf("[+]Client disconnected.\n\n");

  return 0;
}