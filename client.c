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

    int32_t num = TO_INT(2013);
    // s_write_size(socket, TO_SOCKET_MESSAGE(num), sizeof(int32_t));
    s_write(socket, TO_SOCKET_MESSAGE(num));
    printf("sended number: %d\n", num);

    while (1);

    closesocket(socket);

    return 0;
}