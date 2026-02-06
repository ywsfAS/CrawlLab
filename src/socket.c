#include "../include/error.h"
#include "../include/socket.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/* Create a TCP socket  */
int socket_create(int family, int type) {
    set_context("socket_create");

    if (family == 0) family = AF_INET;
    if (type == 0) type = SOCK_STREAM;

    int fd = socket(family, type, 0);
    if (fd < 0)
        fatal_error(ERR_SOCKET_CREATE, "socket() failed");

    return fd;
}

/* Load IPv4 sockaddr  */
void load_sockaddr_ipv4(struct sockaddr_in *addr, uint16_t port, const char *ipv4) {
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);

    if (ipv4) {
        if (inet_pton(AF_INET, ipv4, &addr->sin_addr) != 1) {
            fatal_error(ERR_INVALID_ADDRESS, "Invalid IPv4 address");
        }
    } else {
        addr->sin_addr.s_addr = INADDR_ANY;
    }
}

/* Load IPv6 sockaddr */
void load_sockaddr_ipv6(struct sockaddr_in6 *addr, uint16_t port, const char *ipv6) {
    memset(addr, 0, sizeof(*addr));
    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);

    if (ipv6) {
        if (inet_pton(AF_INET6, ipv6, &addr->sin6_addr) != 1) {
            fatal_error(ERR_INVALID_ADDRESS, "Invalid IPv6 address");
        }
    } else {
        addr->sin6_addr = in6addr_any;
    }
}

/* Bind socket (IPv4)   */
int socket_bind_IPv4(int sockfd, uint16_t port, const char *ipv4, int is_fatal) {
    set_context("socket_bind_IPv4");

    struct sockaddr_in addr;
    load_sockaddr_ipv4(&addr, port, ipv4);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        if (is_fatal)
            fatal_error(ERR_SOCKET_BIND, "IPv4 bind failed");
        error(ERR_SOCKET_BIND, "IPv4 bind failed");
        return 0;
    }

    return 1;
}

/* Bind socket (IPv6) */
int socket_bind_IPv6(int sockfd, uint16_t port, const char *ipv6, int is_fatal) {
    set_context("socket_bind_IPv6");

    struct sockaddr_in6 addr;
    load_sockaddr_ipv6(&addr, port, ipv6);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        if (is_fatal)
            fatal_error(ERR_SOCKET_BIND, "IPv6 bind failed");
        error(ERR_SOCKET_BIND, "IPv6 bind failed");
        return 0;
    }

    return 1;
}

/* Listen */
int socket_listen(int sockfd, int backlog) {
    set_context("socket_listen");

    if (sockfd < 0 || backlog < 0)
        return 0;

    if (listen(sockfd, backlog) < 0)
        fatal_error(ERR_SOCKET_LISTEN, "listen() failed");

    return 1;
}

/* Accept  */
int socket_accept(int sockfd, int is_fatal) {
    set_context("socket_accept");

    if (sockfd < 0)
        return -1;

    int client_fd = accept(sockfd, NULL, NULL);
    if (client_fd < 0) {
        if (is_fatal)
            fatal_error(ERR_SOCKET_ACCEPT, "accept() failed");

        error(ERR_SOCKET_ACCEPT, "accept() failed");
        return -1;
    }

    return client_fd;
}

/* Connect (IPv4 or IPv6) */
int socket_connect(int sockfd, const char *ip, int port) {
    set_context("socket_connect");

    if (sockfd < 0 || !ip || port <= 0)
        fatal_error(ERR_SOCKET_CONNECT, "invalid arguments");

    /* IPv4 */
    struct sockaddr_in addr4;
    if (inet_pton(AF_INET, ip, &addr4.sin_addr) == 1) {
        load_sockaddr_ipv4(&addr4, port, ip);

        if (connect(sockfd, (struct sockaddr *)&addr4, sizeof(addr4)) < 0)
            fatal_error(ERR_SOCKET_CONNECT, "IPv4 connect failed");

        return 1;
    }

    /* IPv6 */
    struct sockaddr_in6 addr6;
    if (inet_pton(AF_INET6, ip, &addr6.sin6_addr) == 1) {
        load_sockaddr_ipv6(&addr6, port, ip);

        if (connect(sockfd, (struct sockaddr *)&addr6, sizeof(addr6)) < 0)
            fatal_error(ERR_SOCKET_CONNECT, "IPv6 connect failed");

        return 1;
    }

    fatal_error(ERR_SOCKET_CONNECT, "invalid IP address format");
    return 0;
}

/* Send / send_all  */
ssize_t socket_send(int sockfd, const void *buffer, size_t size) {
    set_context("socket_send");

    if (sockfd < 0 || !buffer || size == 0)
        return -1;

    ssize_t sent = send(sockfd, buffer, size, 0);
    if (sent < 0) {
        error(ERR_SOCKET_SEND, "send() failed");
        return -1;
    }

    return sent;
}

int socket_send_all(int sockfd, const void *buffer, size_t size) {
    set_context("socket_send_all");

    if (sockfd < 0 || !buffer || size == 0)
        return 0;

    size_t total_sent = 0;
    const char *buf = (const char *)buffer;

    while (total_sent < size) {
        ssize_t sent = send(sockfd, buf + total_sent, size - total_sent, 0);
        if (sent <= 0) {
            error(ERR_SOCKET_SEND, "client disconnected");
            return 0;
        }
        total_sent += sent;
    }

    return 1;
}

/* Recv / recv_all */
ssize_t socket_recv(int sockfd, void *buffer, size_t size) {
    set_context("socket_recv");

    if (sockfd < 0 || !buffer || size == 0)
        return -1;

    ssize_t received = recv(sockfd, buffer, size, 0);
    if (received < 0) {
        error(ERR_SOCKET_RECV, "recv() failed");
        return -1;
    }

    return received;
}

int socket_recv_all(int sockfd, void *buffer, size_t size) {
    set_context("socket_recv_all");

    if (sockfd < 0 || !buffer || size == 0)
        return 0;

    size_t total_received = 0;
    char *buf = (char *)buffer;

    while (total_received < size) {
        ssize_t received = recv(sockfd, buf + total_received, size - total_received, 0);
        if (received <= 0) {
            error(ERR_SOCKET_RECV, "client disconnected");
            return 0;
        }
        total_received += received;
    }

    return 1;
}

/* Close / shutdown  */
int socket_close(int sockfd) {
    set_context("socket_close");

    if (sockfd < 0)
        return 0;

    if (close(sockfd) < 0) {
        error(ERR_SOCKET_CLOSE, "close() failed");
        return 0;
    }

    return 1;
}

int socket_shutdown(int sockfd, int shutdown_mode) {
    set_context("socket_shutdown");

    if (sockfd < 0)
        return 0;

    if (shutdown(sockfd, shutdown_mode) < 0) {
        error(ERR_SOCKET_SHUTDOWN, "shutdown() failed");
        return 0;
    }

    return 1;
}

/* Non-blocking helpers */
int socket_get_flags(int sockfd, int *flags) {
    if (!flags) return -1;

    int f = fcntl(sockfd, F_GETFL, 0);
    if (f < 0) return -1;

    *flags = f;
    return 0;
}

int socket_set_nonblocking(int sockfd) {
    set_context("socket_set_nonblocking");

    int flags;
    if (socket_get_flags(sockfd, &flags) < 0) {
        error(ERR_SOCKET_SETFLAGS, "F_GETFL failed");
        return 0;
    }

    flags |= O_NONBLOCK;

    if (fcntl(sockfd, F_SETFL, flags) < 0) {
        error(ERR_SOCKET_SETFLAGS, "F_SETFL failed");
        return 0;
    }

    return 1;
}

/* Convenience listener */
int get_listener_socket(const char *port_str, int backlog) {
    set_context("get_listener_socket");

    struct addrinfo hints = {0}, *res = NULL, *p = NULL;
    int yes = 1;
    int listener_socket = -1;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port_str, &hints, &res) != 0)
        fatal_error(ERR_SOCKET_BIND, "getaddrinfo() failed");

    for (p = res; p != NULL; p = p->ai_next) {
        listener_socket = socket_create(p->ai_family, p->ai_socktype);

        if (setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
            socket_close(listener_socket);
            continue;
        }

        if (bind(listener_socket, p->ai_addr, p->ai_addrlen) == 0)
            break;

        socket_close(listener_socket);
        listener_socket = -1;
    }

    freeaddrinfo(res);

    if (listener_socket < 0)
        fatal_error(ERR_SOCKET_BIND, "No addresses available to bind");

    socket_listen(listener_socket, backlog);
    return listener_socket;
}

/* Convenience client connect */
int connect_to_server(const char* ip, const char* port_str) {
    set_context("connect_to_server");

    if (!ip || !port_str)
        fatal_error(ERR_SOCKET_CONNECT, "Invalid IP or port");

    struct addrinfo hints = {0}, *res = NULL, *p = NULL;
    int sockfd = -1;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(ip, port_str, &hints, &res) != 0)
        error(ERR_SOCKET_CONNECT, "getaddrinfo() failed");

    for (p = res; p != NULL; p = p->ai_next) {
        sockfd = socket_create(p->ai_family, p->ai_socktype);

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0)
            break;

        socket_close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(res);

    if (sockfd < 0)
        error(ERR_SOCKET_CONNECT, "Unable to connect to server");

    return sockfd;
}

