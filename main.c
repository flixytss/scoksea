#include "scok.h"
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    struct tcp socket;

    init_tcp(&socket, 8080);
    set_tcp_addr(&socket, OPENADDRESS);
    bind_tcp(&socket, 1);

    printf("waiting\n");
    while (1) {
        struct tcp client;
        get_connect(socket, &client);

        struct tcpclient info;
        set_tcp_struct(&client, &info);

        char buffer[100];
        s_read(client, buffer);

        printf("%s:%d: %s\n", info.ip, info.port, buffer);

        closesocket(client);
    }

    closesocket(socket);

    return 0;
}