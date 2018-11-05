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
#define MAX_VAL 10

typedef struct kv { // Key value pair
    char key[MAX_KEY_SIZE];
    char val[MAX_VALUE_SIZE];
} kv_entry;

typedef struct values { // All values that are mapped to the same key
    kv_entry values[MAX_VAL];
    int LRU_idx; // Least recently used index
} key_values;

typedef struct kv_pod { // Pods
    key_values distinct_keys[MAX_VAL];
    int insert_idx; // Next insertion index
    int LRU_idx; // Least recently used index
} pod;

typedef struct kv_store { // Store
    pod pods[NUM_OF_PODS];
} store;

#define KV_SIZE sizeof(kv_entry) + 32 * sizeof(char)
#define KV_SIZE2 sizeof(key_values) + MAX_VAL * KV_SIZE
#define POD_SIZE sizeof(pod) + MAX_VAL * KV_SIZE2
#define STORE_SIZE sizeof(store) + NUM_OF_PODS * POD_SIZE


int kv_store_create(char *kv_store_name);
int kv_store_write(char *key, char *value);
char *kv_store_read(char *key);
char **kv_store_read_all(char *key);

#endif //_A2_LIB_
