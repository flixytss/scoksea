#include "scok.h"

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

int get_last_event() { return global_args._event; }
int s_wait(si_socket s_socket, int action) {
    if (!s_socket._socket) return -1;

    int epfd = epoll_create1(0);
    struct epoll_event event;
    event.data.fd = s_socket._socket;
    event.events = action;
    epoll_ctl(epfd, EPOLL_CTL_ADD, s_socket._socket, &event);

    int nfds = epoll_wait(epfd, &event, 1, global_args._timeout);
    global_args._event = event.events;
    close(epfd);
    return nfds > 0;
}
int set_socket_addr(si_socket* s_socket, const char* addr) {
    if (strcmp(addr, OPENADDRESS) == 0) {
        s_socket->_socketaddr.sin_addr.s_addr = global_args._mode == UDP ? htonl(INADDR_ANY) : INADDR_ANY;
        return 0;
    }
    if ((inet_pton(AF_INET, addr, &s_socket->_socketaddr.sin_addr)) <= 0) { if (global_args._perror) perror("inet_pton"); return -1; }
    return 0;
}
int init_socket(si_socket* s_socket, const char* ip, unsigned long port, int mode) {
    global_args._perror = 1;
    global_args._timeout = -1;
    global_args._mode = mode;
    global_args._global._socket = -1; /* global_args is only for clients in udp mode */
    global_args._errno = 0;

    if (global_args._mode == UDP) {
        bzero(&s_socket->_socketaddr, sizeof(s_socket->_socketaddr));
        /* bzero(&global_args._global._socketaddr, sizeof(global_args._global._socketaddr)); */
    }

    if ((s_socket->_socket = socket(AF_INET, mode == TCP ? SOCK_STREAM : SOCK_DGRAM, 0)) < 0) { if (global_args._perror) perror("socket"); return -1; }
    s_socket->_socketaddr.sin_port = htons(port);
    s_socket->_socketaddr.sin_family = AF_INET;

    int opt = 1;
    if (setsockopt(s_socket->_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { if (global_args._perror) perror("setsockopt"); return -1; }

    /*
        int flags = fcntl(tcp->_socket, F_GETFL, 0);
        if (flags == -1) { perror("fcntl"); return -1; }
        if (fcntl(tcp->_socket, F_SETFL, flags | O_NONBLOCK) == -1) { perror("fcntl"); return -1; }

        Enable non-blocking socket, Tells the system to stop the accept if it late or if it dosen't get any
        new connection. For efficient, this is disabled
    */

    set_socket_addr(s_socket, ip);

    return 0;
}
int bind_socket(si_socket* s_socket, uint16_t max) {
    s_socket->_len = sizeof(s_socket->_socketaddr);
    if (!s_socket->_socket) return -1;
    if (bind(s_socket->_socket, (struct sockaddr*)&s_socket->_socketaddr, sizeof(s_socket->_socketaddr)) < 0) { if (global_args._perror) perror("socket"); return -1; }
    if (global_args._mode != UDP) if (listen(s_socket->_socket, max) < 0) { if (global_args._perror) perror("listen"); return -1; }
    return 0;
}
si_socket get_connect(si_socket s_socket) {
    if (global_args._mode == UDP) {
        puts("udp does not support accept\n");
        return (si_socket){};
    }

    si_socket buffer;
    buffer._len = sizeof(buffer._socketaddr);
    if ((buffer._socket = accept(s_socket._socket, (struct sockaddr*)&buffer._socketaddr, &buffer._len)) < 0) { if (global_args._perror) perror("accept"); return (struct s_info){}; }
    return buffer;
}
int connect_socket(si_socket* s_socket) {
    s_socket->_len = sizeof(s_socket->_socketaddr);
    if ((connect(s_socket->_socket, (struct sockaddr*)&s_socket->_socketaddr, s_socket->_len)) < 0) { if (global_args._perror) perror("connect"); return -1; }
    /* checking if the server is exists, only in UDP */
    if (global_args._mode == UDP) {
        send(s_socket->_socket, MAGIC_BYTES_UDP, MAGIC_BYTES_UDP_SIZE, 0);
        int timeout_original = global_args._timeout;
        set_timeout(0500); /* has a 1 second delay when connecting to the udp server */
        s_wait(*s_socket, INE);
        if (get_last_event() & ERR) {
            int err;
            socklen_t len = sizeof(err);
            getsockopt(s_socket->_socket, SOL_SOCKET, SO_ERROR, &err, &len);
            if (global_args._perror == 1 && err == ECONNREFUSED) puts("no response from the udp server");
            global_args._errno = err;
            set_timeout(timeout_original);
            return -1;
        }
        set_timeout(timeout_original);
    }

    return 0;
}

ssize_t write_all(si_socket s_socket, void *buf, size_t len) {
    size_t total = 0;
    ssize_t n;
    const int fd = s_socket._socket;
    if (!fd) return -1;

    while (total < len) {
        if (global_args._mode != UDP) { n = send(fd, (const char *)buf + total, len - total, MSG_NOSIGNAL); }
        else {
            global_args._global._len = sizeof(global_args._global._socketaddr);
            n = sendto(fd, (const char *)buf + total, len - total, 0, (struct sockaddr*)&global_args._global._socketaddr, sizeof(global_args._global._socketaddr));
        }
        if (n < 0) {
            if (errno == EPIPE) { if (global_args._perror) puts("broken pipe (the server or client disconnected)"); }
            return errno;
        }
        if (n == 0) break;
        total += n;
    }
    return total;
}
ssize_t read_all(si_socket s_socket, void *buf, size_t len) {
    size_t total = 0;
    ssize_t n;
    const int fd = s_socket._socket;
    if (!fd) return -1;

    while (total < len) {
        if (global_args._mode != UDP) { n = recv(fd, (char *)buf + total, len - total, 0); }
        else { /* fucking udp */
            n = recv(fd, (char *)buf + total, len - total, 0);
            if (strncmp(buf, MAGIC_BYTES_UDP, MAGIC_BYTES_UDP_SIZE) == 0) { memcpy(buf, "magic", 5); return 0; }
        }

        if (n < 0) {
            if (errno == EINTR) continue;
            if (errno == EPIPE) { if (global_args._perror) puts("broken pipe (the server or client disconnected)"); }
            return errno;
        }
        if (n == 0) break;
        total += n;
    }
    return total;
}
int s_read(si_socket s_socket, void* buf, int max_size) {
    if (s_wait(s_socket, IN)) return read_all(s_socket, buf, max_size);
    return -1;
}
int ss_read(si_socket s_socket, void* buf, int max_size) { return read_all(s_socket, buf, max_size); }
int s_write(si_socket s_socket, void* buf, int siz) { return write_all(s_socket, buf, siz); }
struct s_client get_client_struct(si_socket* socket) {
    struct s_client tcp;
    if (inet_ntop(AF_INET, &socket->_socketaddr.sin_addr, tcp.ip, INET_ADDRSTRLEN) == NULL) { if (global_args._perror) perror("inet_ntop"); return (struct s_client){}; }
    tcp.port = ntohs(socket->_socketaddr.sin_port);
    return tcp;
}
void silence_errors() { global_args._perror = 0; }
void set_timeout(unsigned long timeout) { global_args._timeout = timeout; }
/* Returns -1 as fd because recvfrom only returns the sockaddr_in.
*/ struct s_info* s_get_global() {
    global_args._global._len = sizeof(global_args._global._socketaddr);
    return &global_args._global;
}
void closesocket(si_socket s_socket) {
    close(s_socket._socket);
    s_write_with_size(s_socket, (void*)DISCONNECTMSG, DISCONNECTMSG_SIZE);
    shutdown(s_socket._socket, SHUT_WR);
}
int s_write_with_size(si_socket socket, void* buf, int size) {
    int cons = htonl(size);
    s_write(socket, &cons, sizeof(cons));
    return s_write(socket, buf, size);
}
int s_read_with_size(si_socket socket, void* buf) {
    int size;
    s_read(socket, &size, sizeof(size));
    return s_read(socket, buf, ntohl(size));
}