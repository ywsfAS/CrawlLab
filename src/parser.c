#include<string.h>
#include<stdio.h>
#include <stdbool.h>
#include "../include/parser.h"
#include "../include/stats.h"

void url_list_init(url_list_t * list , int capacity){
    list->url = calloc(capacity,sizeof(char *));
    if(!list->url){
        perror("calloc failed");
        return;
    }
    list->count = 0;
    list->capacity = capacity;
}
void url_list_destroy(url_list_t * list){
    for(int i = 0 ; i < list->count ; i++){
        free(list->url[i]);
    }
    free(list->url);
    list->count = 0;
    list->capacity = 0;

}
bool is_disallowed(url_list_t *disallow, const char *url) {
    for (int i = 0; i < disallow->count; i++) {
        if (strncmp(url, disallow->url[i], strlen(disallow->url[i])) == 0) {
            return true; // blocked
        }
    }
    return false; // allowed
}
bool add_url(url_list_t *list, char *url) {
    if (list->count >= list->capacity) {
        char **tmp = realloc(list->url, list->capacity * 2 * sizeof(char *));
        if (!tmp) {
            perror("realloc failed");
            return false;
        }
        list->url = tmp;
        list->capacity *= 2;
    }

    // Copy the string properly
    list->url[list->count] = strdup(url);  // allocates a copy of url
    if (!list->url[list->count]) {
        perror("strdup failed");
        return false;
    }

    stats_inc(&stats.urls_discovered , &stats.lock);
    list->count++;
    return true;
}

void parse_robots(url_list_t *list, const char *robots_txt) {
    if(!robots_txt) return ;
    const char *line = robots_txt;
    while (*line) {
        // Find end of line
        const char *eol = strchr(line, '\n');
        size_t len = eol ? (size_t)(eol - line) : strlen(line);

        // Check if line starts with "Disallow:"
        if (len > 9 && strncasecmp(line, "Disallow:", 9) == 0) {
            const char *path = line + 9;
            while (*path == ' ' || *path == '\t') path++; // skip spaces
            if (*path != '\0') {
                add_url(list, path); // add to list
            }
        }

        // Move to next line
        line = eol ? eol + 1 : line + len;
    }
}

void parse_links(url_list_t *list, const char *html) {
    const char *ptr = html;
    while ((ptr = strstr(ptr, "<a href=\"")) != NULL) { 
        ptr += 9; // start of URL after <a href="
        const char *end = strchr(ptr, '"');
        if (!end) break;

        int len = end - ptr;
        char *new_url = malloc(len + 1);
        if (!new_url) {
            perror("malloc failed");
            break;
        }
        strncpy(new_url, ptr, len);
        new_url[len] = '\0';

        add_url(list, new_url);
        free(new_url); // add_url strdup's the string
        ptr = end + 1; // move pointer past the last "
    }
}



