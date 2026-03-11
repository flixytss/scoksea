# ScokSea

This library simplify the sockets use in C/C++. The files main.c and client.c is just an example of the use of this library

## Usage

server.c:
``` c
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
        s_read(client, TO_SOCKET_MESSAGE(num));

        printf("%s %d %d\n", info.ip, info.port, FROM_INT(num));

        closesocket(client);
    }

    closesocket(socket);

    return 0;
}
```
Here we start declaring the main socket variable with the type of a struct named `tcp`, this is the start to use our socket. First we init the socket with the port we want it to be in, then we set the socket address (`OPENADDRESS` for bind it to any address), and finally we bind the socket (if it is the server side) with the number of max connections we want (if the connections exceed the max, they will get refused).  
Here we already initialized the socket, now we're listening, if you want to listen to connections always, then use a while. First we start declaring the client variable with the same type as the main socket (`struct tcp`) because it is a socket more, then we get the connection from the socket and set it to the client variable pointer (with the get_connect), it will stop there until there's a in coming connection, if we want the client's data (ip and port) we will declare a variable that will hold the client's data, with the type `struct tcpclient`, tcpclient structure:
``` c
struct tcpclient {
    char ip[INET_ADDRSTRLEN];
    uint16_t port;
};
```
With that, we get the client's data with `set_tcp_struct` and pass it the client pointer and the variable pointer that will hold the data