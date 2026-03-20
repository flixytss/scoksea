#include "scok.h"
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/* This is only for an example
*/ typedef struct vector2 {
    int x;
    int y;
} vector2;

int main() {
    struct s_info socket;

    init_socket(&socket, OPENADDRESS, 5065, TCP); // OPENADDRESS set the socket to any address
    bind_socket(&socket, 1); // Bind the socket to just one listen

    printf("waiting\n");

    struct s_info client = get_connect(socket); /* The program stays there until there is a new connection */
    struct s_client info = get_client_struct(&client); /* Get the client's information (ip and port) */

    printf("connected from : %s:%d\n", info.ip, info.port);

    // Writting down
    {
        char* buffer = "Hello there!";

        s_write(client, buffer, strlen(buffer)); /* Send the wanted buffer to the client's socket */
        s_write_with_size(client, buffer, strlen(buffer)); /* Sends first the size of the buffer and then the buffer so s_read_with_size can catch it */
    }

    // Reading
    {
        char buffer[100];

        s_read(client, buffer, 100); // Read any in-coming message and write it down to the buffer with a max capacity of SIZ
        s_read_with_size(client, buffer); // Read any in-coming message sended with s_write_with_size
    }

    // Miscellaneous
    if (s_wait(client, IN)) printf("There is in-coming data!\n"); // Stop there until it detect in-coming data in the socket
    ss_read(client, NULL, 0); // Read without the epoll security
    set_socket_addr(&socket, "127.0.0.1"); // Set the socket address; this is for the initialization of the socket, and, it no longer needed
    set_timeout(2000); // Set the timeout that the epoll is gonna use
    silence_errors(); // Silence inside errors

    // Need to close the sockets that ain't gonna use anymore
    closesocket(client);
    closesocket(socket);

    return 0;
}