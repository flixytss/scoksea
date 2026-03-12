#include "scok.h"

#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int init_tcp(tcpsocket* tcp, unsigned long port) {
    if ((tcp->_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) { perror("socket"); return -1; }
    tcp->_socketaddr.sin_port = htons(port);
    tcp->_socketaddr.sin_family = AF_INET;

    int opt = 1;
    if (setsockopt(tcp->_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { perror("setsockopt"); return -1; }
    return 0;
}
int set_tcp_addr(tcpsocket* tcp, const char* addr) {
    if (strcmp(addr, OPENADDRESS) == 0) {
        tcp->_socketaddr.sin_addr.s_addr = INADDR_ANY;
        return 0;
    }
    if ((inet_pton(AF_INET, addr, &tcp->_socketaddr.sin_addr)) <= 0) { perror("inet_pton"); return -1; }
    return 0;
}
int bind_tcp(tcpsocket* tcp, uint16_t max) {
    tcp->_len = sizeof(tcp->_socketaddr);
    if (bind(tcp->_socket, (struct sockaddr*)&tcp->_socketaddr, sizeof(tcp->_socketaddr)) < 0) { perror("socket"); return -1; }
    if (listen(tcp->_socket, max) < 0) { perror("listen"); return -1; }
    return 0;
}
int get_connect(tcpsocket tcp, tcpsocket* buffer) {
    buffer->_len = sizeof(buffer->_socketaddr);
    if ((buffer->_socket = accept(tcp._socket, (struct sockaddr*)&buffer->_socketaddr, &buffer->_len)) < 0) { perror("accept"); return -1; }
    return 0;
}
int connect_socket(tcpsocket* tcp) {
    tcp->_len = sizeof(tcp->_socketaddr);
    if ((connect(tcp->_socket, (struct sockaddr*)&tcp->_socketaddr, tcp->_len)) < 0) { perror("connect"); return -1; }
    return 0;
}

ssize_t write_all(tcpsocket tcp, void *buf, size_t len) {
    size_t total = 0;
    ssize_t n;
    const int fd = tcp._socket;

    while (total < len) {
        n = write(fd, (const char *)buf + total, len - total);
        if (n <= 0) return -1;
        total += n;
    }
    return total;
}
ssize_t read_all(tcpsocket tcp, void *buf, size_t len) {
    size_t total = 0;
    ssize_t n;
    const int fd = tcp._socket;

    while (total < len) {
        n = read(fd, (char *)buf + total, len - total);

        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        };
        if (n == 0) break;
        total += n;
    }
    return total;
}
int s_read_size(tcpsocket tcp, void* buf, int siz) { return read_all(tcp, buf, siz); }
int s_read(tcpsocket tcp, void* buf) {
    int32_t ssize = 0;
    read_all(tcp, TO_SOCKET_MESSAGE(ssize), sizeof(ssize));
    return read_all(tcp, buf, ssize);
}
int s_write(tcpsocket tcp, void* buf, int siz) {
    int32_t ssize = TO_INT(siz);
    write_all(tcp, TO_SOCKET_MESSAGE(ssize), sizeof(ssize));
    return write_all(tcp, buf, siz);
}
int set_tcp_struct(tcpsocket* socket, struct tcpclient* tcp) {
    if (inet_ntop(AF_INET, &socket->_socketaddr.sin_addr, tcp->ip, INET_ADDRSTRLEN) == NULL) { perror("inet_ntop"); return -1; }
    tcp->port = ntohs(socket->_socketaddr.sin_port);
    return 0;
}

void closesocket(tcpsocket tcp) { close(tcp._socket); }