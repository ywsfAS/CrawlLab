#ifndef PARSER_H
#define PARSER_H
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

typedef struct {
    char **url;
    int count;
    int capacity;
}url_list_t;
void url_list_init(url_list_t * list , int capacity);
void url_list_destroy(url_list_t * list);
bool add_url(url_list_t * list , char *url);
void parse_links(url_list_t *list , const char *source);


#endif
