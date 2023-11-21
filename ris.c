#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
int server_sock;
socklen_t addr_size;
#define BUFFER_SIZE 1000
// #define MAX_PATH_LENGTH 1000
typedef char* string;
typedef struct details
{
  char ip[100];
  int port_no;
  string list_of_accesible_paths[200];
  int client_port_no;
} details;
typedef struct tokenised{
  string* buffer;
}tokenised;
details ss_dets[100];
int ss_count=0;
int client_sock[1000];
struct sockaddr_in client_addr[1000];
#define CACHE_SIZE 10

typedef struct CacheNode {
    char* path;
    // char* data;
    struct CacheNode* next;
    struct CacheNode* prev;
} CacheNode;

typedef struct {
    int size;
    int capacity;
    CacheNode* head;
    CacheNode* tail;
} LRUCache;

LRUCache* createCache() {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->size = 0;
    cache->capacity = CACHE_SIZE;
    cache->head = NULL;
    cache->tail = NULL;
    return cache;
}

void addToCache(LRUCache* cache, char* path, char* data) {
    CacheNode* newNode = (CacheNode*)malloc(sizeof(CacheNode));
    newNode->path = strdup(path);
    // newNode->data = strdup(data);
    newNode->next = NULL;
    newNode->prev = NULL;

    if (cache->size == 0) {
        cache->head = newNode;
        cache->tail = newNode;
        cache->size++;
        return;
    }

    // Remove the oldest entry if the cache is full
    if (cache->size >= cache->capacity) {
        CacheNode* temp = cache->tail;
        cache->tail = temp->prev;
        if (cache->tail != NULL) {
            cache->tail->next = NULL;
        }
        free(temp->path);
        // free(temp->data);
        free(temp);
        cache->size--;
    }

    // Add the new entry to the front
    newNode->next = cache->head;
    if (cache->head != NULL) {
        cache->head->prev = newNode;
    }
    cache->head = newNode;
    cache->size++;
}

char* fetchFromCache(LRUCache* cache, char* path) {
    CacheNode* curr = cache->head;

    while (curr != NULL) {
        if (strcmp(curr->path, path) == 0) {
            // Move the accessed node to the front (most recently used)
            if (curr != cache->head) {
                if (curr == cache->tail) {
                    cache->tail = curr->prev;
                    cache->tail->next = NULL;
                } else {
                    curr->prev->next = curr->next;
                    curr->next->prev = curr->prev;
                }
                curr->next = cache->head;
                curr->prev = NULL;
                cache->head->prev = curr;
                cache->head = curr;
            }
            return curr->path; // Return the cached data
        }
        curr = curr->next;
    }

    return NULL; // Data not found in cache
}


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
char *ip = "127.0.0.1";
int port = 5000;


void handle_client(char* buffer,int count){
  char *tokens=malloc(sizeof(char)*1000);
  for (int i=0;i<1000;i++){
    tokens[i]=malloc(sizeof(char)*1000);
  }
  tokens=gettokens(buffer);
  string req_path=tokens[1];
  int i;
  int index;
  for ( i = 0; i < ss_count; i++)
  {
    index=-1;
    int j=0;
    while (1)
    {
      if (strcmp(ss_dets[i].list_of_accesible_paths[j],req_path)==0)
      {
        index=j;
        break;
      }
      j++;
    }
  }
  if (strcmp(tokens[0],"READ")==0||strcmp(tokens[0],"WRITE")==0||strcmp(tokens[0],"GET_DETAILS")==0)
  {
    int ss_port=ss_dets[index].port_no;
    char* ip_add=ss_dets[index].ip;
    client_sock[count] = accept(server_sock, (struct sockaddr *)&client_addr[count], &addr_size);
    int n = send(client_sock[count], ip_add, strlen(ip_add), 0);
    if (n<0)
    {
      perror("send");
    }
    n = send(client_sock[count], &ss_port, sizeof(ss_port), 0);
    if (n<0)
    {
      perror("send");
    }
    //wait for ack from ss.
  }
  else if (strcmp(tokens[0],"COPY")==0)
  {
    string dest_path=tokens[2];
    int i2;
    int index2;
    for ( i2 = 0; i2 < ss_count; i2++)
    {
      index2=-1;
      int j2=0;
      while (1)
      {
        if (strcmp(ss_dets[i2].list_of_accesible_paths[j2],dest_path)==0)
        {
          index2=j2;
          break;
        }
        j2++;
      }
    }
    int dest_port=ss_dets[index2].port_no;
    int ss_port=ss_dets[index].port_no;
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
    int n = connect(new_sock, (struct sockaddr*)&new_addr, sizeof(new_addr));
    if (n < 0) {
      perror("Connect error");
      exit(1);
    }
    printf("Connected to the naming server.\n");
    string dest;
    dest=(string)malloc(sizeof(char)*1000);
    sprintf(dest, "%d", dest_port);
    strcpy(tokens[3],dest);
  
  tokenised token;
  token.buffer=tokens;
  if( send(new_sock,(void*)&token, sizeof(token),0) < 0 ) {
      printf("send failed!\n");
  }
      perror("Send error");
    // close(sock);
    }
  
  else{
  int ss_port=ss_dets[index].port_no;
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
  int n = connect(new_sock, (struct sockaddr*)&new_addr, sizeof(new_addr));
  if (n < 0) {
    perror("Connect error");
    exit(1);
  }
  printf("Connected to the naming server.\n");
tokenised token;
token.buffer=tokens;
if( send(new_sock,(void*)&token, sizeof(token),0) < 0 ) {
    printf("send failed!\n");
}
    perror("Send error");
  // close(sock);
  }
 
}

void *receive_buffer(void *arg) {
  // Code to receive buffer from client
  char buffer[1024];
  // Error handling for listen() call
  listen(server_sock, 1000);
  int count=0;
  while (1)
  {
    addr_size = sizeof(client_addr[count]);
    client_sock[count] = accept(server_sock, (struct sockaddr *)&client_addr[count], &addr_size);
    // Error handling for recv() call
    bzero(buffer, 1024);
    recv(client_sock[ss_count],buffer, sizeof(buffer), 0);
    count++;
    handle_client(buffer,count);
  }
}


void handleClientWithCache(char* buffer, int count, LRUCache* cache) {
    char** tokens = malloc(sizeof(char*) * 1000);
    for (int i = 0; i < 1000; i++) {
        tokens[i] = malloc(sizeof(char) * 1000);
    }
    tokens = gettokens(buffer);
    string req_path = tokens[1];
    int i;
    int index;
    for (i = 0; i < ss_count; i++) {
        index = -1;
        int j = 0;
        while (1) {
            if (strcmp(ss_dets[i].list_of_accesible_paths[j], req_path) == 0) {
                index = j;
                break;
            }
            j++;
        }
    }

    if (strcmp(tokens[0], "READ") == 0 || strcmp(tokens[0], "WRITE") == 0 || strcmp(tokens[0], "GET_DETAILS") == 0) {
        char* cachedData = fetchFromCache(cache, req_path);
        if (cachedData != NULL) {
            // Data found in cache (cache hit)
            printf("Cache Hit: Returning data from cache to the client...\n");
            send(client_sock[count],cachedData, strlen(cachedData), 0);
        } else {
            // Data not found in cache (cache miss)
            printf("Cache Miss: Proceeding with request handling...\n");
            int ss_port = ss_dets[index].port_no;
            char* ip_add = ss_dets[index].ip;

            client_sock[count] = accept(server_sock, (struct sockaddr *)&client_addr[count], &addr_size);
            int n = send(client_sock[count], ip_add, strlen(ip_add), 0);
            if (n < 0) {
                perror("send");
            }
            n = send(client_sock[count], &ss_port, sizeof(ss_port), 0);
            if (n < 0) {
                perror("send");
            }
            // Wait for acknowledgment from the storage server.
            // On receiving the data, update the cache.
            // Example: char receivedData[BUFFER_SIZE];
            // recv(client_sock[count], receivedData, sizeof(receivedData), 0);
            // addToCache(cache, req_path, receivedData);
        }
    } else {
        // Handle other types of requests without using cache
        printf("Handling other types of requests without using cache...\n");
        // Example: Send the request directly to the storage server without caching
    }
}


int main()
{
  // pthread_t thread1, thread2;
  // int ret1, ret2;
  // ret1 = pthread_create(&thread1, NULL, &receive_buffer, NULL);
  // ret2 = pthread_create(&thread2, NULL, &accept_ss, NULL);
  // if (ret1)
  // {
  //   printf("Error: pthread_create() failed\n");
  //   exit(EXIT_FAILURE);
  // }
  // if (ret2)
  // {
  //   printf("Error: pthread_create() failed\n");
  //   exit(EXIT_FAILURE);
  // }
  int  ss_sock[1000];
  struct sockaddr_in server_addr, ss_addr[1000];
  char buffer[1024];
  int n;

  // Error handling for socket() call
  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0)
  {
    perror("[-]Socket error");
        exit(1);
  }
  printf("[+]TCP server socket created.\n");

  // Error handling for bind() call
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

  // Error handling for listen() call
  listen(server_sock, 1000);
  details nm_details;
  memset(&nm_details, '\0', sizeof(struct details));

  int ss_count = 0;
  while (1)
  {
    addr_size = sizeof(ss_addr[ss_count]);
    ss_sock[ss_count] = accept(server_sock, (struct sockaddr *)&ss_addr[ss_count], &addr_size);
    printf("ss %d connected.\n", ss_count);
    memset(&ss_dets[ss_count], '\0', sizeof(struct details));

    // Error handling for recv() call
    bzero(buffer, 1024);
    recv(ss_sock[ss_count], &ss_dets[ss_count], sizeof(details), 0);
    printf("ss %d ip: %s\n", ss_count, ss_dets[ss_count].ip);
    printf("ss %d port_no: %d\n", ss_count, ss_dets[ss_count].port_no);
    int i=0;
    while (ss_dets[ss_count].list_of_accesible_paths[i]!=NULL)
    {
      nm_details.list_of_accesible_paths[i] = malloc(sizeof(char)*1000);
      printf("ss %d list_of_paths: %s\n", ss_count, ss_dets[ss_count].list_of_accesible_paths[i]);
      i++;
    }
    
    printf("ss %d client_port_no: %d\n", ss_count, ss_dets[ss_count].client_port_no);
    n = send(ss_sock[ss_count], buffer, strlen(buffer), 0);
    if (n < 0)
    {
      perror("Send error");
      exit(1);
    }
    printf("data sent\n");
    ss_count++;
    if (getchar()=='\n'){
      break;
    }
  }
  
  // close(ss_sock);
  printf("[+]Client disconnected.\n\n");

  return 0;
}