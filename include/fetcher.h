#ifndef FETCHER_H
#define FETCHER_H
#include "socket.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define HTTP_PORT "80"

char * fetch_page(const char * url);
#endif
