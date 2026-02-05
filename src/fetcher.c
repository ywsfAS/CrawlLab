#include "../include/fetcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define HTTP_PORT "80"
#define HTTPS_PORT 443

char* fetch_http(const char *host, const char *path) {
    int sockfd = connect_to_server(host, HTTP_PORT);
    if (sockfd < 0) return NULL;

    char request[1024];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
             path, host);

    if (!socket_send_all(sockfd, request, strlen(request))) {
        socket_close(sockfd);
        return NULL;
    }

    size_t buffer_size = 0;
    char *buffer = NULL;
    char temp[1024];
    ssize_t bytes;

    while ((bytes = socket_recv(sockfd, temp, sizeof(temp))) > 0) {
        char *new_buffer = realloc(buffer, buffer_size + bytes + 1);
        if (!new_buffer) {
            free(buffer);
            socket_close(sockfd);
            return NULL;
        }
        buffer = new_buffer;
        memcpy(buffer + buffer_size, temp, bytes);
        buffer_size += bytes;
    }

    if (buffer) buffer[buffer_size] = '\0';

    socket_close(sockfd);
    return buffer;
}
char* fetch_https(const char *host, const char *path) {
    SSL_CTX *ctx = NULL;
    SSL *ssl = NULL;
    int sockfd = -1;
    char *buffer = NULL;
    size_t buffer_size = 0;

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) return NULL;

    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    if (!SSL_CTX_set_default_verify_paths(ctx)) {
        SSL_CTX_free(ctx);
        return NULL;
    }

    // Correct: use string for port
    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, "443", &hints, &res) != 0 || !res) {
        SSL_CTX_free(ctx);
        return NULL;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        freeaddrinfo(res);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        close(sockfd);
        freeaddrinfo(res);
        SSL_CTX_free(ctx);
        return NULL;
    }
    freeaddrinfo(res);

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) <= 0) {
        SSL_free(ssl);
        close(sockfd);
        SSL_CTX_free(ctx);
        return NULL;
    }

    char req[1024];
    snprintf(req, sizeof(req),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             path, host);

    if (SSL_write(ssl, req, strlen(req)) <= 0) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sockfd);
        SSL_CTX_free(ctx);
        return NULL;
    }

    char temp[1024];
    int bytes;
    while ((bytes = SSL_read(ssl, temp, sizeof(temp))) > 0) {
        char *new_buffer = realloc(buffer, buffer_size + bytes + 1);
        if (!new_buffer) {
            free(buffer);
            SSL_shutdown(ssl);
            SSL_free(ssl);
            close(sockfd);
            SSL_CTX_free(ctx);
            return NULL;
        }
        buffer = new_buffer;
        memcpy(buffer + buffer_size, temp, bytes);
        buffer_size += bytes;
    }

    if (buffer) buffer[buffer_size] = '\0';

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);

    return buffer;
}
char* fetch_page(const char *url) {
    char host[256] = {0};
    char path[1024] = "/";

    if (strncmp(url, "http://", 7) == 0) {
        if (sscanf(url, "http://%255[^/]%1023[^\n]", host, path) < 1) {
            fprintf(stderr, "Invalid HTTP URL: %s\n", url);
            return NULL;
        }
        return fetch_http(host, path);
    } 
    else if (strncmp(url, "https://", 8) == 0) {
        if (sscanf(url, "https://%255[^/]%1023[^\n]", host, path) < 1) {
            fprintf(stderr, "Invalid HTTPS URL: %s\n", url);
            return NULL;
        }
        return fetch_https(host, path);
    } 
    else {
        fprintf(stderr, "URL must start with http:// or https://: %s\n", url);
        return NULL;
    }
}

