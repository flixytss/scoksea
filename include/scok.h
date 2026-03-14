#ifndef SCOK_H
#define SCOK_H

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define OPENADDRESS "0.0.0.0"

#define TO_INT(int) htonl(int)
#define FROM_INT(int) ntohl(int)
#define TO_SOCKET_MESSAGE(int) &int

struct tcp {
    int _socket;
    struct sockaddr_in _socketaddr;
    socklen_t _len;
};
typedef struct tcp tcpsocket;

struct tcpclient {
    char ip[INET_ADDRSTRLEN];
    uint16_t port;
};

extern int init_tcp(tcpsocket *, unsigned long);
extern int set_tcp_addr(tcpsocket *, const char *);
extern int bind_tcp(tcpsocket *, uint16_t);
extern int connect_socket(tcpsocket *);
extern int get_connect(tcpsocket, tcpsocket *);
extern int set_tcp_struct(tcpsocket *, struct tcpclient *);
extern int s_write(tcpsocket tcp, void* buf, int siz);
extern int s_read(tcpsocket tcp, void* buf, int siz);

extern void closesocket(tcpsocket);

#endif