#include "scok.h"
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct vector2 {
    int x;
    int y;
} vector2;

int main() {
    struct s_info socket;

    init_tcp(&socket, OPENADDRESS, 5065, UDP);
    bind_tcp(&socket, 1);

    char buffer[4];

    printf("waiting\n");
    while (1) {
        // if (s_wait(socket, IN)) ss_read(*s_get_global(), "GG!", 3);
        // get_connect(socket);

        s_read(socket, buffer, 3);
        printf("info: %s\n", buffer);
        struct s_client info = get_client_struct(s_get_global());
        printf("connected from : %s:%d\n", info.ip, info.port);
    }

    closesocket(socket);

    return 0;
}