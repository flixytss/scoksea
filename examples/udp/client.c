#include "scok.h"

typedef struct vector2 {
    int x;
    int y;
} vector2;

int main() {
    struct s_info socket;

    // The main usage information is in the server c file

    init_socket(&socket, "127.0.0.1", 5065, UDP);
    connect_socket(&socket); // Connect to the socket information (the servers address, port and mode)

    // ...

    closesocket(socket);

    return 0;
}