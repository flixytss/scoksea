#include "scok.h"
#include <stdio.h>
#include <string.h>

/* This is only for an example
*/ typedef struct vector2 {
    int x;
    int y;
} vector2;

int main() {
    struct s_info socket;

    init_socket(&socket, OPENADDRESS, 5065, UDP); // OPENADDRESS set the socket to any address

    char buffer[256];
    s_read(*s_get_global(), buffer, 256); // Have to read for in-coming to detect new clients with s_get_global
    struct s_client info = get_client_struct(s_get_global()); // s_get_global is the global socket to register new connections

    printf("connected from : %s:%d\n", info.ip, info.port);

    // Writting down
    {
        char* buffer = "Hello there!";

        s_write(*s_get_global(), buffer, strlen(buffer)); /* Send the wanted buffer to the client's socket */
        s_write_with_size(*s_get_global(), buffer, strlen(buffer)); /* Sends first the size of the buffer and then the buffer so s_read_with_size can catch it */
    }

    // Reading
    {
        char buffer[100];

        s_read(*s_get_global(), buffer, 100); // Read any in-coming message and write it down to the buffer with a max capacity of SIZ
        s_read_with_size(*s_get_global(), buffer); // Read any in-coming message sended with s_write_with_size
    }

    // Miscellaneous
    if (s_wait(*s_get_global(), IN)) printf("There is in-coming data!\n"); // Stop there until it detect in-coming data in the socket
    ss_read(*s_get_global(), NULL, 0); // Read without the epoll security
    set_socket_addr(&socket, "127.0.0.1"); // Set the socket address; this is for the initialization of the socket, and, it no longer needed
    set_timeout(2000); // Set the timeout that the epoll is gonna use
    silence_errors(); // Silence inside errors

    // Need to close the sockets that ain't gonna use anymore
    closesocket(socket);

    return 0;
}