# ScokSea

This library simplify the sockets use in C/C++. The files server.c and client.c is just an example of the use of this library

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
    struct s_info socket;

    init_socket(/* socket pointer */ &socket, /* address */ OPENADDRESS, /* port */ 5065, /* socket mode (TCP or UDP) */ TCP);
    // set_tcp_addr(&socket, OPENADDRESS); This is now obsolete
    bind_tcp(/* socket pointer */ &socket, /* max listenings */ 1);

    printf("waiting\n");
    while (1) {
        struct s_info client = get_connect(socket); /* wait for a client */
        struct s_client info = get_client_struct(&client); /* get client's information */

        char buff[100];
        s_read(client, buff, 100);

        printf("%s:%d: %d\n", info.ip, info.port, buff);

        closesocket(client);
    }

    closesocket(socket);

    return 0;
}
```
Here we start declaring the main socket variable with the type of a struct named `s_info`, this is the start to use our socket. First we init the socket with the port we want it to be in, then we set the socket address (`OPENADDRESS` for bind it to any address), and finally we bind the socket (if it is the server side) with the number of max connections we want (**if the connections exceed the max, they will get refused**).  

Here we already initialized the socket, now we're listening, if you want to listen to connections always, then use a while. First we start declaring the client variable with the same type as the main socket (`struct s_info`) because it is a socket more, then we get the connection from the socket and set it to the client variable pointer (with the get_connect), it will stop there until there's a in-coming connection, if we want the client's data (ip and port) we will declare a variable that will hold the client's data, with the type `struct s_client`. s_client structure:
``` c
struct tcpclient {
    char ip[INET_ADDRSTRLEN];
    uint16_t port;
};
```
With that, we get the client's data with `get_client_struct` and pass it the client pointer and the variable pointer that will hold the data.  
Now we get to the fun part, sending and getting messages, to get a message, you will need a variable that will hold it and call `s_read` with the socket you want to send it and the information you want to send, to send a array of chars, do:
``` c
    char buffer[/*size*/];
    s_read(client, buffer, /*size*/);
```
**s_read will pause the program until there is in-coming data in the socket**, See Miscellaneous section for more information.

Now, after learning how to read, we will write to a socket, how to send messages to the client socket, now you'll use `s_write` like this:
``` c
    const char buffer[] = "Hello, World!";
    s_write(client, buffer, /*size*/);
```
To send structures, ints, floats, etc.... You have to send it as binary, with the Address-of operator (&) like this: `s_write(client, &/* the variable */, sizeof(/* the variable */));`.  

In the client-side, It's almost the same thing, just change the `bind_tcp(&socket, 1);` for `connect_socket(&socket);` and it will connect to the socket that has the ip setted before with `init_tcp` (`set_socket_addr` is now obsolete).  

**After using a socket, you need to free it with `closesocket` and pass it the tcp socket**.

### Udp

The previus section was the Tcp usage. This is the Udp section, The Udp socket is very similar to the Tcp socket. Just replace the `get_connect` with `s_read` because Udp is a proccess that is a connectionless protocol, in s_read use the socket `*s_get_global()` (See Miscellaneous section for more information), and then the s_get_global's socket will have the client's information and data. usually, the first message that `connect_socket` send, is the `MAGIC_BYTES_UDP` that the client send to know if the server is up, when you catch that message, it will return `magic` as the answer.

### Miscellaneous

There are code examples in the directory called examples.  
There are utils functions that helps you with some stuff, there are:
* s_wait: definition is:
```c
int s_wait(
    si_socket socket,
    int action
)
```
s_wait waits for the action ACTION to occur in the passed socket SOCKET. The program will stop there until the action occur, return 0 if err and 1 to be used in an if conditional, the actions are:  
- IN: detect if there are in-coming information from the socket  
- OUT: detect if there are out-coming information from the socket  
- ERR: detect if there are errors from the socket  
- INE: detect in-coming and errors from the socket  
- HUP: detect if the socket hang up in purpose  
- HUPE: detect if the socket hang up by an error or in purpose  

* ss_read: definition is:
```c
int ss_read(
    si_socket socket,
    void* buf,
    int max_size
)
```
ss_read do the same thing as s_read, but, it dosen't pause the program waiting for in-coming data, if it dosen't catch a value, it will return a random value.  

* set_timeout: definition is:
```c
void set_timeout(
    int timeout
)
```
set_timeout set the global timeout for epoll (s_wait), set the timeout that s_wait is gonna wait for an event, -1 if you want it to late forever until the event to occur.  

* silence_errors: definition is:
```c
void silence_errors()
```
silence_errors silence the errors that the backend prints if an error occur.  

* s_get_global: definition is:
```c
struct s_info* s_get_global()
```
s_get_global returns the global socket for the Udp protocol, It is used for get connections.  

* s_read_with_size: definition is:
```c
int s_read_with_size(
    si_socket socket,
    void* buf
)
```
s_read_with_size reads a message sended with s_write_with_size, the message include the size of itself, so you don't have to specify the size.  

* s_write_with_size: definition is:
```c
int s_write_with_size(
    si_socket socket,
    void* buf,
    int max_size
)
```
s_write_with_size sends a message with the size included.

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