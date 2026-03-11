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

extern void init_tcp(tcpsocket *, unsigned long);
extern void set_tcp_addr(tcpsocket *, const char *);
extern void bind_tcp(tcpsocket *, uint16_t);
extern void connect_socket(tcpsocket *);
extern int s_read_size(tcpsocket, void *, int);
extern int s_write_size(tcpsocket, void *, int);
extern void get_connect(tcpsocket, tcpsocket *);
extern void set_tcp_struct(tcpsocket *, struct tcpclient *);

#define s_read(socket, obj) s_read_size(socket, obj, sizeof(*(obj)))
#define s_write(socket, obj) s_write_size(socket, obj, sizeof(*(obj)))

extern void closesocket(tcpsocket);

#endif