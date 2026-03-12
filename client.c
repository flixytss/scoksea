#include "scok.h"
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    struct tcp socket;

    init_tcp(&socket, 8080);
    set_tcp_addr(&socket, "127.0.0.1");
    connect_socket(&socket);

    printf("connected\n");

    char buffer[] = "Hello, World!";
    s_write(socket, buffer, sizeof(buffer));

    closesocket(socket);

    return 0;
}