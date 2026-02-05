#ifndef VISITED_SET_H
#define VISITED_SET_H
#include "hash_table.h"

bool visited_contains(hash_table_t * visited , const char *url);
bool visited_add(hash_table_t * visited , const char *url);
bool visited_check_and_add(hash_table_t *visited, const char *url);
#endif

