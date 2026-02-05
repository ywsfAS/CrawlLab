#include "../include/hash_table.h"
#include <string.h>

hash_table_t * hash_table_create(uint32_t size , hash_function_t * hash_func){
    hash_table_t * hash_table = malloc(sizeof(hash_table_t));
    hash_table->size = size;
    hash_table->hash = hash_func;
    hash_table->elements = calloc(size,sizeof(entry *));
    pthread_mutex_init(&hash_table->lock,NULL);
    return hash_table;
}
uint64_t hash(const char * key , size_t length){
    uint64_t hash_value = 0;
    for(uint32_t i = 0 ; i < length ; i++){
        hash_value = key[i];
        hash_value *= key[i];
    }
    return hash_value;

}


void hash_table_destroy(hash_table_t * hash_table){
    if(hash_table == NULL) return;
    for(uint32_t i = 0 ; i < hash_table->size ; i++){
        entry * current = hash_table->elements[i];
        while(current != NULL){
            entry * next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    
    }
    
    pthread_mutex_destroy(&hash_table->lock);
    free(hash_table->elements);
    free(hash_table);

}
void hash_table_print(hash_table_t * hash_table ){

   for(uint32_t i = 0 ; i < hash_table->size ; i ++){
        if(hash_table->elements[i] == NULL){
            printf("[Empty Slot]"); 
        }
        else{
            entry * current = hash_table->elements[i];
            while(current != NULL)
            {
                printf("Key (%s) -- Address (%p)");
                current = current->next;
            }
        
        }
        printf("\n");

   } 

}
// private helper for this file
static uint32_t hash_table_index(hash_table_t * hash_table , const char * key){
    return hash_table->hash(key,strlen(key)) % hash_table->size;
}
bool hash_table_insert(hash_table_t * hash_table , const char *key, void * obj){
    if(hash_table == NULL || key == NULL || obj == NULL ) return false;
    uint32_t index = hash_table_index(hash_table,key);
    if(hash_table_lookup(hash_table,key) != NULL ) return false;

    entry * entry = malloc(sizeof(*entry));
    entry->obj = obj;
    entry->key = malloc(strlen(key) + 1);
    strcpy(entry->key,key);

    entry->next = hash_table->elements[index];
    hash_table->elements[index] = entry;
    return true;


}
void * hash_table_lookup(hash_table_t * hash_table , const char * key){
    if(hash_table == NULL || key == NULL) return NULL;
    uint32_t index = hash_table_index(hash_table , key);
    entry * tmp = hash_table->elements[index];
    while(tmp != NULL && strcmp(tmp->key ,key) != 0 ){
        tmp = tmp->next; 
    }
    if(!tmp) return NULL;
    return tmp->obj;

}

void * hash_table_delete(hash_table_t * hash_table , const char * key){
    if(hash_table == NULL || key == NULL) return NULL;
    uint32_t index = hash_table_index(hash_table , key);
    entry * tmp = hash_table->elements[index];
    entry * prev = NULL;
    while(tmp != NULL && strcmp(tmp->key , key) != 0 ){
        prev = tmp;
        tmp = tmp->next; 
    }
    if(!tmp) return NULL;
    if(!prev){
        hash_table->elements[index] = tmp->next;
    }     
    else{
        prev->next = tmp->next;
    }
    void * content = tmp->obj;
    free(tmp->key);
    free(tmp);
    return content;
}


















