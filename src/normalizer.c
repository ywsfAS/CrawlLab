#include "../include/normalizer.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

void remove_fragment(char **url , const char symbol){
    char *index = strchr(*url,symbol); // find index of fragment
    if(!index){ 
        return; // we didnt find any fragment url is correct 
    
    }
    int len = index - *url;
    char * normalized_url = malloc(len + 1); 
    strncpy(normalized_url , *url , len);
    normalized_url[len] = '\0';
    free(*url);
    *url = normalized_url;


}
void remove_symbol_at_end(char **url , const char symbol){
    int len = strlen(*url);
    if(len == 0){
        return;
    }
    char last_char = (*url)[len - 1];
    if(last_char != symbol){
        return;
    }
    char * normalized_url = malloc(len);
    strncpy(normalized_url , *url , len - 1);
    normalized_url[len - 1] = '\0';
    free(*url);
    *url = normalized_url;

}

bool is_absolute(const char * url){
    return strncmp(url , "http://" ,7) == 0 || strncmp(url , "https://" , 8) == 0;
}
char * extract_base(const char * url){
    char * index = strstr(url , "://");
    if(!index){
        return NULL;
    }
    char * end = strchr(index + 3, '/');
    if(!end){
        return strdup(url); // copy string 
    }
    int len = end - url;
    char* base = malloc(len + 1);
    strncpy(base , url  , len );
    base[len] = '\0';
    return base;
}
char  * extract_base_dir(const char *url){
    const char * p = strchr(url + 8 , '/');
    if(!p) return strdup("/");
    int len = strlen(p);
    char * dir = strdup(p);

    char *last_slash = strrchr(dir , '/');
    if(last_slash) *(last_slash + 1) = '\0';
    return dir;
}
char *resolve_relative(const char *base, const char *url) {
    if (is_absolute(url)) {
        return strdup(url); // already absolute
    }

    char *origin = extract_base(base);
    char *base_dir = extract_base_dir(base);

    char path[4096];
    if (url[0] == '/') {
        snprintf(path, sizeof(path), "%s", url); // root relative
    } else {
        snprintf(path, sizeof(path), "%s%s", base_dir,url); // relative
    }


    char *absolute = malloc(strlen(origin) + strlen(path) + 1);
    sprintf(absolute, "%s%s", origin, path);

    free(origin);
    free(base_dir);
    return absolute;
}

char *normalize_url(const char *base, const char *url) {
    char *abs = resolve_relative(base,url);

    remove_fragment(&abs, '#');
    remove_fragment(&abs, '?'); 
    remove_symbol_at_end(&abs, '/');

    return abs;
}








