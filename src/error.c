#include <stdio.h>
#include <stdlib.h>
#include "../include/error.h"

static const char *context = NULL;

/* Map custom SocketError codes to strings */
const char *socket_strerror(SocketError error_code) {
    switch (error_code) {
        case ERR_SOCKET_CREATE:   return "Socket Creation Failed";
        case ERR_SOCKET_BIND:     return "Socket Binding Failed";
        case ERR_SOCKET_LISTEN:   return "Socket Listening Failed";
        case ERR_SOCKET_ACCEPT:   return "Socket Accept Failed";
        case ERR_SOCKET_CONNECT:  return "Socket Connect Failed";
        case ERR_SOCKET_SEND:     return "Socket Send Failed";
        case ERR_SOCKET_RECV:     return "Socket Receive Failed";
        case ERR_SOCKET_TIMEOUT:  return "Socket Operation Timed Out";
        case ERR_SOCKET_SHUTDOWN: return "Socket Shutdown Failed";
        case ERR_INVALID_ADDRESS: return "Invalid IP/Port Address";
        case ERR_PROTOCOL_MISMATCH: return "Protocol Mismatch";
        case ERR_MEMORY_ALLOC:    return "Memory Allocation Failed";
        case ERR_UNKNOWN:         return "Unknown Error";
        case ERR_OK:              return "No Error";
        default:                  return "Unrecognized Error Code";
    }
}

/* Check if error number is a custom SocketError */
int is_custom_error(int error_no) {
    return (error_no >= ERR_SOCKET_CREATE && error_no <= ERR_UNKNOWN);
}

/* Set the current context (function/module) */
void set_context(const char *new_context) {
    context = new_context;
}

/* Print a non-fatal error */
void error(int error_no, const char *msg) {
    if (context) {
        printf("[%s] ", context);
    }

    if (is_custom_error(error_no)) {
        printf("%s: %s\n",
               socket_strerror(error_no),
               msg ? msg : "");
    } else {
        perror(msg);
    }
}

/* Print a fatal error and exit */
void fatal_error(int error_no, const char *msg) {
    error(error_no, msg);
    exit(EXIT_FAILURE);
}

