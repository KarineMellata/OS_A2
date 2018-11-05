//
// Created by Karine Mellata on 2018-11-03.
//

#ifndef _A2_LIB_
#define _A2_LIB_

#define SHAREDMEM_NAME 260741673
#define NUM_OF_PODS 300
#define MAX_KEY_SIZE 32
#define MAX_VALUE_SIZE 256
#define MAX_POD_ENTRIES 256
#define MAX_VAL 16

typedef struct kv_pod { // Pods
    char distinct_keys[MAX_VALUE_SIZE][MAX_VAL];
    char vals[MAX_VALUE_SIZE][MAX_KEY_SIZE];
    int cache_valid; // Next insertion index
    sem_t protect;
} pod;

typedef struct kv_store { // Store
    pod pods[NUM_OF_PODS];
    int clients;
    char name[32];
    sem_t protect;
} store;

#define POD_SIZE sizeof(pod)
#define STORE_SIZE sizeof(store)


int kv_store_create(char *kv_store_name);
int kv_store_write(char *key, char *value);
char *kv_store_read(char *key);
char **kv_store_read_all(char *key);

#endif //_A2_LIB_