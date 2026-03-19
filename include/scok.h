#ifndef SCOK_H
#define SCOK_H

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>

#define OPENADDRESS "0.0.0.0"

struct s_info {
    /* fd */ int _socket;
    /* sockaddr_in */ struct sockaddr_in _socketaddr;
    socklen_t _len;
};
typedef struct s_info si_socket;
struct s_client {
    char ip[INET_ADDRSTRLEN];
    uint16_t port;
};
// Backend arguments
struct args {
    int _perror;
    int _timeout;
    int _mode;
    int _errno;
    /* only for s_wait */ int _event;
    struct s_info _global;
}; struct args global_args;

extern int init_tcp(si_socket* socket, const char* ip, unsigned long port, int mode);
extern int set_tcp_addr(si_socket* socket, const char* addr);
extern int bind_tcp(si_socket* socket, uint16_t port);
extern int connect_socket(si_socket* socket);
extern si_socket get_connect(si_socket socket);
extern struct s_client get_client_struct(si_socket* socket);
extern int s_write(si_socket socket, void* buf, int siz);
extern int s_read(si_socket socket, void* buf, int siz);
extern void set_timeout(unsigned long timeout);
extern void silence_errors();

/*  Pauses the program until the socket fd does the wanted action with the global timeout.
*/ extern int s_wait(si_socket socket, int action);
/* An insecure version of s_read, this disables the epoll wait (wait for the message).
*/ extern int ss_read(si_socket socket, void* buf, int max_size);
/* Returns the global si_socket used in the backend (Use only when the socket is UDP)
*/ extern struct s_info* s_get_global();
/* Returns the last event like IN, OUT or ERR (Only for UDP)
*/ extern int get_last_event();

extern void closesocket(si_socket);

// Wait actions
#define IN  EPOLLIN
#define OUT EPOLLOUT
#define ERR EPOLLERR
/* listen for errors too
*/ #define INE (IN | ERR)

// UDP connect message
const char MAGIC_BYTES_UDP[] = { (char)0x2, (char)0x0, (char)0x1, (char)0x3, (char)0x6, (char)0x11 };
#define MAGIC_BYTES_UDP_SIZE    sizeof  MAGIC_BYTES_UDP

// TCP UDP
#define TCP 0
#define UDP 1

#endif