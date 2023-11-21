#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include<time.h>
int char_to_index(char ch){
    if (ch>='a'&&ch<='z'){
        return ch-'a';
    }
    if (ch>='A'&&ch<='Z'){
        return ch-'A'+26;
    }
    if (ch>='0'&&ch<='9'){
        return ch-'0'+52;
    }
    if (ch=='/'){
        return 62;
    }
    if (ch=='.'){
        return 63;
    }
}
#define CACHE_SIZE 10

typedef struct CacheNode {
    char* path;
    char* data;
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
    newNode->data = strdup(data);
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
        free(temp->data);
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
typedef struct trie{
    struct trie*child[64];
    bool is_end;
}trie;
trie *makenode(){
    trie*flag=malloc(sizeof (trie));
    flag->is_end=false;
    for (int i=0;i<64;i++){
        flag->child[i]=NULL;
    }
    return flag;
}
void insert(trie*root,char *str){
    int len=strlen(str);
    int index;
    struct trie *flag=root; 
    for (int i=0;i<len;i++){
        index=char_to_index(str[i]);
        flag->child[index]=makenode();
        flag=flag->child[index];
    }
    flag->is_end=true;
}
bool search(trie*root,char *str){
    trie*flag=malloc(sizeof (trie));
    int index;
    flag=root;
    for (int i=0;i<strlen(str);i++){
        index=char_to_index(str[i]);
        if (flag->child[index]==NULL){
            return false;
        }
        flag=flag->child[index];
    }
    return flag->is_end;
}

// Define a structure to store IP address, port, and log messages
typedef struct LogRecord {
    char ip[15];
    int port;
    char logMessage[100]; // Adjust the message length as needed
    int status;
    struct LogRecord *next;
} LogRecord;

// Create a global linked list head pointer
LogRecord *logHead = NULL;

// Function to add log records to the linked list
void add_log(const char *ip, int port, const char *logMessage,int status) {
    LogRecord *newRecord = (LogRecord *)malloc(sizeof(LogRecord));
    if (newRecord == NULL) {
        // Handle memory allocation error
        return;
    }
    strncpy(newRecord->ip, ip, sizeof(newRecord->ip) - 1);
    newRecord->ip[sizeof(newRecord->ip) - 1] = '\0';
    newRecord->port = port;
    newRecord->status=0;
    strncpy(newRecord->logMessage, logMessage, sizeof(newRecord->logMessage) - 1);
    newRecord->logMessage[sizeof(newRecord->logMessage) - 1] = '\0';
    newRecord->next = NULL;

    if (logHead == NULL) {
        logHead = newRecord;
    } else {
        LogRecord *temp = logHead;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newRecord;
    }
}
void update_log(int port, int newStatus) {
    LogRecord *temp = logHead;
    while (temp != NULL) {
        if (temp->port == port) {
            temp->status = newStatus;
            return; // Assuming each port is unique, so we can exit early
        }
        temp = temp->next;
    }
}
// Function to display logs
void display_logs() {
    LogRecord *temp = logHead;
    printf("IP Address\tPort\tLog Message\tstatus\n");
    while (temp != NULL) {
        printf("%s\t%d\t%s\n", temp->ip, temp->port, temp->logMessage);
        temp = temp->next;
    }
}

// Define error codes
typedef enum {
    ERROR_NONE = 0,                  // No error
    ERROR_FILE_NOT_FOUND = 404,      // File not found error
    ERROR_FILE_IN_USE = 409,         // File is currently in use error
    ERROR_PERMISSION_DENIED = 403,   // Permission denied error
    // Add more error codes as needed for different scenarios
} ErrorCode;

