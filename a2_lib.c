//
// Created by Karine Mellata on 2018-11-01.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "a2_lib.h"
#include "comp310_a2_test.h"

//Hashing function taken from tutorial
int hash(unsigned char *str) {
    unsigned long hash = 0;
    int c;

    while (c == *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    hash = (hash > 0) ? hash % NUM_OF_PODS : -(hash) % NUM_OF_PODS;

    return (int) hash;
}

int last_idx(char arr[]){
    return sizeof(arr)/sizeof(arr[0]);
}

//Initialize bookkeeping information
int init_info(store* ptr) {
    memset(ptr, 0, sizeof(store));
    int idx;
    //Init pods
    for(idx = 0; idx < NUM_OF_PODS; idx++){
        (ptr->pods[idx]).insert_idx = 0;
        (ptr->pods[idx]).LRU_idx = 0;
        int i;
        //Init all LRU objects
        for(i = 0; i < 10; i++){
            ((ptr->pods[idx]).distinct_keys[i]).LRU_idx = 0;
        }
    }
    return 0;
}

//Create a store if not yet created
//Or open store if already existing
int kv_store_create(char *name){
    int fd = shm_open(name, O_CREAT|O_RDWR, S_IRWXU); //All permission for owner
    if(fd < 0){
        perror("Error ");
        return -1;
    }

    if(ftruncate(fd, strlen(store)) < 0){
        close(fd);
        perror("Error ");
        return -1;
    }

    store *addr = mmap(NULL, sizeof(store) , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(addr == MAP_FAILED){
        close(fd);
        perror("Error ");
        return -1;
    }
    init_info(addr);

    if(munmap(addr, sizeof(store)) < 0){
        close(fd);
        perror("Error ");
        return -1;
    }

    close(fd);
    return 0;
}

int get_key_idx(char* key, int hashed_key, store *sm) {
    pod key_pod = sm->pods[hashed_key];
    int idx;

    for(idx = 0; idx < last_idx(key_pod.distinct_keys); idx++) {
        if(strcmp((((key_pod.distinct_keys[idx]).values[0]).key), key) == 0)
            return idx;
    }

    return -1;
}
int insert(char* key, char* value, store* ptr, int hashed_key) {
    kv_entry entry;
    memset(entry.key, 0, sizeof(entry.key));
    memset(entry.val, 0, sizeof(entry.val));
    strncpy(entry.key, key, strlen(key));
    strncpy(entry.val, value, strlen(value));


    int idx = (ptr->pods[hashed_key]).insert_idx;
    int dist_key_idx = last_idx((ptr->pods[hashed_key]).distinct_keys[idx].values);
    if(dist_key_idx >= (MAX_VAL - 1)){
        dist_key_idx = 0;
    }
    (ptr->pods[hashed_key]).distinct_keys[idx].values[dist_key_idx] = entry;
    (ptr->pods[hashed_key]).insert_idx++;

    if((ptr->pods[hashed_key]).insert_idx >= (MAX_VAL - 1)){
        (ptr->pods[hashed_key]).insert_idx = 0;
    }

    return 0;
}

int modify_entry(char* key, char* value, key_values key_vals) {
    int i;
    for(i = 0; i < last_idx(key_vals.values); i++) {
        if(key_vals.values[i].key[0] == '\0') {
            fflush(stdout);
        }
    }
    return 0;
}
//Take a key-value pair and write to the store
int kv_store_write(char *key, char *value){
    int key_len = strlen(key);
    int val_len = strlen(value);
    char new_key[MAX_KEY_SIZE + 1];
    char new_val[MAX_VALUE_SIZE + 1];
    int last_key_idx = (key_len > __TEST_MAX_KEY_SIZE__) ? MAX_KEY_SIZE : key_len;
    int last_val_idx = (val_len > __TEST_MAX_DATA_LENGTH__) ? MAX_VALUE_SIZE : val_len;
    
    strncpy(new_key, key, last_key_idx);
    new_key[last_key_idx] = '\0';
    
    strncpy(new_val, value, last_val_idx);
    new_val[last_val_idx] = '\0';
    
    int fd = shm_open(__TEST_SHARED_MEM_NAME__, O_CREAT|O_RDWR, S_IRWXU); //All permission for owner
    if(fd < 0){
        perror("Error ");
        return -1;
    }

    store *addr = mmap(NULL, sizeof(store) , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(addr == MAP_FAILED){
        close(fd);
        perror("Error ");
        return -1;
    }

    if(ftruncate(fd, strlen(store)) < 0){
        close(fd);
        perror("Error ");
        return -1;
    }

    int hashed_key = hash(key);
    int idx = get_key_idx(key, hashed_key, addr);

    insert(new_key, new_val, addr, hashed_key);

    if(munmap(addr, sizeof(store)) < 0){
        close(fd);
        perror("Error ");
        return -1;
    }
    close(fd);
    return 0;

}

//Find key and return copy of the value
char *kv_store_read(char *key){
    int fd = shm_open(__TEST_SHARED_MEM_NAME__, O_CREAT|O_RDWR, S_IRWXU); //All permission for owner
    if(fd < 0){
        perror("Error ");
        return -1;
    }

    store *addr = mmap(NULL, sizeof(store) , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(addr == MAP_FAILED){
        close(fd);
        perror("Error ");
        return -1;
    }

    if(ftruncate(fd, strlen(store)) < 0){
        close(fd);
        perror("Error ");
        return -1;
    }

    char *value = NULL;

    int hashed_key = hash(key);
    pod key_pod = (addr->pods[hashed_key]);
    int i;
    for(i = 0; i < MAX_VAL; i++){
        key_values curr_entry = key_pod.distinct_keys[i];
        if(hashed_key == hash(curr_entry.values[0].key)){
            value = (char *) calloc(1, sizeof(char) * 32);
            strcpy(value, curr_entry.values[curr_entry.LRU_idx]);
            curr_entry.LRU_idx++;
            if(curr_entry.LRU_idx >= MAX_VAL - 1){
                curr_entry.LRU_idx = 0;
            }
            break;
        }
    }

    if(munmap(addr, sizeof(store)) < 0){
        close(fd);
        perror("Error ");
        return -1;
    }
    close(fd);
    return value;

}

//Take a key and return all values in store
char **kv_store_read_all(char *key){


}

