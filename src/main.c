#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#include "mysocket.h"
#include "client.h"
#include "noticeboard.h"
#include "util.h"
#include "commands.h"

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

/* Attempt to receive data from `client`. */
void client_exec(Client* client, int i, ClientSet* set,
                 NoticeBoard* noticeboard);

void destroy(ClientSet* set, NoticeBoard* noticeboard);

void disconnect(Client* client);

void send_motd(Client* client);

int main(int argc, char* argv[])
{
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        error("Couldn't catch signal SIGINT");
    }

    Socket server;
    SockAddrIn server_addr;
    ClientSet* clients = ClientSet_Create(8);
    NoticeBoard* board = NoticeBoard_Create();

    /* Attempt to create socket and report error if failed. */
    server = socket(PF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        error("Could not open server socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    fcntl(server, F_SETFL, O_NONBLOCK);
    int optval = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

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
        usleep(100000);
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
            int j = 0;
            fcntl(new_socket, F_SETFL, O_NONBLOCK);
            Client* c = ClientSet_Add(clients, new_socket);
            if (c == NULL) continue;
            memset(c->properties.nick, 0, NICK_MAXLEN);
            strcpy(c->properties.nick, "user");

            for (i = 0; i < clients->size; i++) {
                if (&clients->clients[i] == c) continue;
                if (!strcmp(clients->clients[i].properties.nick,
                    c->properties.nick)) {
                    j++;
                    sprintf(c->properties.nick, "user(%d)", j);
                    i = 0;
                }
            }

            SockAddr peer_address;
            socklen_t addrlen = sizeof(peer_address);
            int has_error = getpeername(new_socket, &peer_address,
                                &addrlen);
            if (has_error == -1) {
                error("Error getting peer name");
            }
            SockAddrIn *addr_in = (struct sockaddr_in *)&peer_address;

            printf("Client connected (%s)\n", ipaddr(*addr_in));
            send_motd(c);
        }

        /* -- Reading data -- */
        for (i = 0; i < clients->size; i++) {
            if (clients->clients[i].state == DISCONNECTED) {
                close(clients->clients[i].socket);
                continue;
            }
            client_exec(&(clients->clients[i]), i, clients, board);
        }
    }
}

void client_exec(Client* client, int i, ClientSet* clients, NoticeBoard* board)
{
    char buffer[512];
    memset(buffer, 0, 512);
    if (difftime(time(NULL), client->last_recv) > 30 * 60) {
        printf("Client timed out.\n");
        disconnect(client);
        return;
    }

    int bytes_recv = recv(client->socket, buffer, 511, 0);
    if ((bytes_recv == 0) || (bytes_recv == -1 && errno != EAGAIN)) {
        disconnect(client);
        //clients->size--;
        //memmove(client, client + sizeof(Client),
        //        *(clients->clients) - client);
    }

    if (bytes_recv != -1) {
        client->last_recv = time(NULL);
    }

    if (starts(buffer, "read")) {
        Command_Read(*client, clients, board);
    } else if (starts(buffer, "post")) {
        Command_Post(*client, clients, board, buffer);
    } else if (starts(buffer, "time")) {
        Command_Time(*client);
    } else if (starts(buffer, "online")) {
        Command_Online(*client, clients);
    } else if (starts(buffer, "nick")) {
        Command_Nick(client, clients, buffer);
    } else if (starts(buffer, "debug")) {
        Command_Debug(client, clients, board);
    } else if (starts(buffer, "help")) {
        Command_Help(*client);
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

void disconnect(Client* client)
{
    client->state = DISCONNECTED;
    SockAddr peer_address;
    socklen_t addrlen = sizeof(peer_address);
    int has_error = getpeername(client->socket, &peer_address,
                        &addrlen);
    if (has_error == -1) {
        error("Error getting peer name");
    }
    SockAddrIn* addr_in = (SockAddrIn*)&peer_address;
    printf("Client disconnected (%s)\n", ipaddr(*addr_in));
}

void send_motd(Client* client)
{
    char motd[1024];
    memset(motd, 0, 1024);
    strcpy(motd,
    "====== MOTD ======\n"
    "Welcome to the notice boards!\n"
    "Enjoy your stay! :-)\n\n"
    "You will automatically disconnect after 30 minutes of inactivity.\n"
    "==================\n");

    Client_Send(*client, motd, 1024);
}
