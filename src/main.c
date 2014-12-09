#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#include "mysocket.h"
#include "client.h"
#include "noticeboard.h"
#include "util.h"

#define PORT 7777

static int running = false;

/* Handle signals. This function currently
 * calls destroy() on SIGINT.
 */
void sig_handler(int signal);

/* Infinite loop until terminated.
 * Both listens and accepts incoming connections nonblocking.
 */
void server_exec(Socket server, SockAddrIn server_addr, ClientSet* clients,
                 NoticeBoard* noticeboard);

/* Attempt to receive data from client. */
void client_exec(Client client, NoticeBoard* noticeboard);

void destroy(ClientSet* set, NoticeBoard* noticeboard);

int main(int argc, char* argv[])
{
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        error("Couldn't catch signal SIGINT");
    }

    Socket server;
    SockAddrIn server_addr;
    ClientSet* clients = ClientSet_Create();
    NoticeBoard* board = NoticeBoard_Create();

    /* Attempt to create socket and report error if failed. */
    server = socket(PF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        error("Could not open server socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    fcntl(server, F_SETFL, O_NONBLOCK);

    /* Set port and attempt to bind. */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (INADDR_ANY) {
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    if (bind(server, (SockAddr*)&server_addr, sizeof(server_addr)) == -1) {
        error("Failed to bind socket");
        exit(1);
    }

    running = true;

    server_exec(server, server_addr, clients, board);
    destroy(clients, board);
    if (close(server) == -1) {
        error("Failed to close server socket?");
    }
    return 0;
}

void server_exec(Socket server, SockAddrIn server_addr, ClientSet* clients,
                 NoticeBoard* board)
{
    int i;

    if (listen(server, 5) == -1) {
        error("Could not listen on socket");
        exit(1);
    }

    while (running) {
        /* -- Accepting connections -- */
        socklen_t sz = sizeof(server_addr);
        Socket new_socket = accept(server, (SockAddr*)&server_addr, &sz);
        /* Error handling, most likely EAGAIN since we're using
         * non blocking sockets.
         */
        if (new_socket == -1) {
            if (errno != EAGAIN) {
                error("Could not accept connection");
            }
        } else {
            fcntl(new_socket, F_SETFL, O_NONBLOCK);
            ClientSet_Add(clients, new_socket);

            struct sockaddr peer_address;
            socklen_t addrlen = sizeof(peer_address);
            int has_error = getpeername(new_socket, &peer_address,
                                &addrlen);
            if (has_error == -1) {
                error("Error getting peer name");
            }
            struct sockaddr_in *addr_in = (struct sockaddr_in *)&peer_address;
        }

        /* -- Reading data -- */
        for (i = 0; i < clients->size; i++) {
            if (clients->clients[i].state == DISCONNECTED) {
                close(clients->clients[i].socket);
                continue;
            }
            client_exec(clients->clients[i], board);
        }
    }
}

void client_exec(Client client, NoticeBoard* board)
{
    char buffer[512];

    memset(buffer, 0, 512);
    int bytes_recv = recv(client.socket, buffer, 511, 0);
    if (bytes_recv == -1) {
        if (errno != EAGAIN) {
            error("Could not recv from client");
            client.state = DISCONNECTED;
        }
        return;
    }

    if (starts(buffer, "read")) {
        char temp_buffer[512];
        Client_Send(client, "-----\n", 7);
        for (int i = 0; i < board->size; i++) {

            memset(temp_buffer, 0, 512);
            snprintf(temp_buffer, 512, "%d: %s", i + 1, board->board[i]);
            Client_Send(client, temp_buffer, 512 - 6);
        }
        Client_Send(client, "-----\n", 7);
    } else if (starts(buffer, "post")) {
        buffer[5 + 134] = 0;
        NoticeBoard_Add(board, buffer + 5);
    }
}

void destroy(ClientSet* set, NoticeBoard* board)
{
    printf("Freeing memory...\n");
    ClientSet_Destroy(set);
    NoticeBoard_Destroy(board);
}

void sig_handler(int signal)
{
    if (signal == SIGINT) {
        running = false;
    }
}
