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

    init_tcp(&socket, "127.0.0.1", 5065, UDP);
    silence_errors();
    connect_socket(&socket);

    printf("connected\n");

    char buffer[4] = "GG!";
    
    s_write(socket, buffer, sizeof(buffer));
    printf("sended: %s\n", buffer);

    closesocket(socket);

    return 0;
}