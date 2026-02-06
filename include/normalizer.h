#ifndef NORMALIZER_H 
#define NORMALIZER_H
#include <stdbool.h>    
void remove_fragment(char **url , const char symbol);
void remove_symbol_at_end(char ** url , const char symbol);
bool is_absolute(const char * url );
char * extract_base(const char * url);
char * extract_base_dir(const char *url);
char * resolve_relative(const char * base , const char * url);
char * normalize_url(const char * base , const char * url);

#endif
