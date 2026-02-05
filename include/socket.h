#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>
#include <netinet/in.h>

#define ERR_BUSY_MSG          "==> [ERROR] ERR_BUSY <==\n"
#define ERR_HOST_MSG          "==> [ERROR] ERR_HOSTNAME_NOT_RESOLVED <==\n"
#define ERR_SOCKET_MSG        "==> [ERROR] ERR_CREATE_SOCKET <==\n"
#define ERR_CONNECT_MSG       "==> [ERROR] ERR_CONNECT <==\n"

/* Create a TCP socket. family=0 → AF_INET, type=0 → SOCK_STREAM. Fatal on failure */
int socket_create(int family, int type);

/* Load IPv4/IPv6 sockaddr structures (NULL IP → all interfaces) */
void load_sockaddr_in_IPv4(struct sockaddr_in *hints, uint16_t port, const char *ipv4);
void load_sockaddr_in_IPv6(struct sockaddr_in6 *hints, uint16_t port, const char *ipv6);

/* Bind socket (is_fatal > 0 → fatal_error, else error only) */
int socket_bind_IPv4(int sockfd, uint16_t port, const char *ipv4, int is_fatal);
int socket_bind_IPv6(int sockfd, uint16_t port, const char *ipv6, int is_fatal);

/* Listen on socket (backlog), fatal on failure */
int socket_listen(int sockfd, int backlog);

/* Accept connection, returns client fd or -1 */
int socket_accept(int sockfd, int is_fatal);

/* Connect socket to IPv4 or IPv6, fatal on failure */
int socket_connect(int sockfd, const char *ip, int port);

/* Send/receive wrappers */
ssize_t socket_send(int sockfd, const void *buffer, size_t size);
int socket_send_all(int sockfd, const void *buffer, size_t size);
ssize_t socket_recv(int sockfd, void *buffer, size_t size);
int socket_recv_all(int sockfd, void *buffer, size_t size);

/* Close or shutdown socket */
int socket_close(int sockfd);
int socket_shutdown(int sockfd, int shutdown_mode);

/* Non-blocking helpers */
int socket_get_flags(int sockfd, int *flags);
int socket_set_nonblocking(int sockfd);

/* Convenience functions */
int get_listener_socket(const char *port_str, int backlog);
int connect_to_server(const char* ip, const char* port_str);

#endif /* SOCKET_H */
