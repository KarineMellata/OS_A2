//
// Created by Karine Mellata on 2018-11-03.
//

#ifndef _A2_LIB_
#define _A2_LIB_

#define SHAREDMEM_NAME 260741673 _KM
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

#endif //_A2_LIB_
