#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <sys/types.h>

#include "mysocket.h"
#include "client.h"

void error(char* message);

void server_exec(Socket server, SockAddr server_addr, ClientSet* clients);

void destroy();

int main(int argc, char* argv[])
{
    Socket server;
    SockAddr server_addr;
    ClientSet* clients = ClientSet_Create();

    /* Attempt to create socket and report error if failed. */
    server = socket(PF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        error("Could not open server socket");
        exit(1);
    }

    fcntl(server, F_SETFL, O_NONBLOCK);

    /* Set port and attempt to bind. */

    if (bind(server, &server_addr, sizeof(server_addr)) == -1) {
        error("Failed to bind socket");
        exit(1);
    }

    server_exec(server, server_addr, clients);
    destroy(clients);
    close(server);
    return 0;
}

void error(char* message)
{
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
}

void server_exec(Socket server, SockAddr server_addr, ClientSet* clients)
{
    if (listen(server, 5) == -1) {
        error("Could not listen on socket");
        exit(1);
    }

    while (true) {
        socklen_t sz = sizeof(server_addr);
        Socket new_socket = accept(server, &server_addr, &sz);
        if (new_socket == -1) {
            if (errno != EAGAIN) {
                error("Could not accept connection");
            }
        }
        fcntl(new_socket, F_SETFL, O_NONBLOCK);
        ClientSet_Add(clients, new_socket);
    }
}

void destroy(ClientSet* set)
{
    ClientSet_Destroy(set);
}
