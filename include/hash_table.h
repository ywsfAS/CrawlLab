#ifndef HASHTABLE_H
#define HASHTABLE_H
#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>
typedef uint64_t hash_function_t (const char *,size_t);
typedef struct {
  char *key;
  void * obj;
  struct entry * next;   // To handle Collision 

}entry;
typedef struct {
 uint32_t size;
 hash_function_t * hash;
 entry **elements;
 pthread_mutex_t lock;
}hash_table_t;

hash_table_t *hash_table_create(uint32_t size , hash_function_t * hash);
void hash_table_destroy(hash_table_t * hash_table);
void hash_table_print(hash_table_t * hash_table);
bool hash_table_insert(hash_table_t * hash_table , const char * key , void * obj);
void * hash_table_lookup(hash_table_t * hash_table , const char * key);
void * hash_table_delete(hash_table_t * hash_table ,const char * key);
uint64_t hash(const char * key , size_t size);
#endif
