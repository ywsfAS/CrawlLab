#include "../include/visited_set.h"
#include<stdbool.h>

bool visited_contains( hash_table_t * visited ,const char * url){
    bool url_found = false;
    pthread_mutex_lock(&visited->lock);
    if(hash_table_lookup(visited , url) != NULL){
        url_found = true;
    }
    pthread_mutex_unlock(&visited->lock);
    return url_found;
}
// false means it was already in the hash table 
bool visited_add(hash_table_t * visited , const char * url ){
    bool result = false;
    pthread_mutex_lock(&visited->lock);
    if(hash_table_lookup(visited,url) == NULL){
        hash_table_insert(visited,url,(void *)1); // set is just a hash table with dummy data 1
        result = true;
    }
    pthread_mutex_unlock(&visited->lock);
    return result ; 
}

bool visited_check_and_add(hash_table_t *visited, const char *url) {
    pthread_mutex_lock(&visited->lock);
    if(hash_table_lookup(visited, url) != NULL) {
        pthread_mutex_unlock(&visited->lock);
        return false; // already visited
    }
    hash_table_insert(visited, url, (void*)1);
    pthread_mutex_unlock(&visited->lock);
    return true; // newly added
}

