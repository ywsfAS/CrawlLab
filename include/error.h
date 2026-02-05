#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

/* Custom project error codes */
typedef enum {
    ERR_OK = 0,                 // No error
    ERR_SOCKET_CREATE,          // Failed to create socket
    ERR_SOCKET_BIND,            // Failed to bind socket
    ERR_SOCKET_LISTEN,          // Failed to listen on socket
    ERR_SOCKET_ACCEPT,          // Failed to accept connection
    ERR_SOCKET_CONNECT,         // Failed to connect to remote
    ERR_SOCKET_SEND,            // Failed to send data
    ERR_SOCKET_RECV,
    ERR_SOCKET_SETFLAGS,
    ERR_SOCKET_CLOSE,				    // Failed to receive data
    ERR_SOCKET_TIMEOUT,         // Operation timed out
    ERR_SOCKET_SHUTDOWN,        // Failed to shutdown socket
    ERR_INVALID_ADDRESS,        // Invalid IP or port
    ERR_PROTOCOL_MISMATCH,      // Protocol error
    ERR_MEMORY_ALLOC,           // Failed to allocate memory
    ERR_UNKNOWN                 // Catch-all for unknown errors
} SocketError;

/* Functions */
const char* socket_strerror(SocketError error);
int is_custom_error(int error_no);
void set_context(const char *new_context);
void error(int error_no, const char *msg);
void fatal_error(int error_no, const char *msg);

#endif /* ERROR_H */
