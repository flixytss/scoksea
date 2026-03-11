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

        int32_t num = 0;
        // s_read_size(client, TO_SOCKET_MESSAGE(num), sizeof(int32_t));
        s_read(client, TO_SOCKET_MESSAGE(num));

        printf("%s %d %d\n", info.ip, info.port, FROM_INT(num));

        closesocket(client);
    }

    closesocket(socket);

    return 0;
}