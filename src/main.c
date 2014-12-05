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

void server_exec(Socket server, SockAddr server_addr, Client client);

int main(int argc, char* argv[]) {
    Socket server;
    SockAddr server_addr;
    Client* clients;

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
    }

    server_exec(server, server_addr);

    close(server);
    return 0;
}

void error(char* message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
}

void server_exec(Socket server, SockAddr server_addr, Client client) {
    while (true) {

    }
}
