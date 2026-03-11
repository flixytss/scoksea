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
With that, we get the client's data with `set_tcp_struct` and pass it the client pointer and the variable pointer that will hold the data.  
Now we get to the fun part, sending a getting messages, to get a message, you will need a variable that will hold it and call `s_read` with the socket you want to send it and the information you want to send, to send a array of chars, do:
``` c
    char buffer[/*size*/];
    s_read(client, buffer);
    /*
        An alternative version of that, is the manual function (s_read_size), that has the same parameters as s_read but it needs a third parameter with the size you want to read
    */
```
If you want to read into an int, you will have to use `TO_SOCKET_MESSAGE` with the int32_t you want to hold the data (It is a macro, that expands to: &your_variable), and finally, to use the int32_t that you getted, use the macro `FROM_INT` with the int32_t and it will make the conversion (`FROM_INT` expands to `ntohl(int32_t)`). 

**After using a socket, you need to free it with `closesocket` and pass it the tcp socket**.

Now, after learning how to read, we will write to a socket, how to send messages to the client socket, now you'll use `s_write` like this:
``` c
    const char buffer[] = "Hello, World!";
    s_write(client, buffer);
    /*
        An alternative version of that, is the manual function (s_write_size), that has the same parameters as s_write but it needs a third parameter with the size you want to write
    */
```

## Building

Do:
``` bash
git clone https://github.com/flixytss/scoksea.git
cd scoksea/build
cmake ..
make
cd ..
sudo cmake --install build
```