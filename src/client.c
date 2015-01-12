#include "client.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>

#include "util.h"

/* Client functions */

void Client_Send(Client client, char* buffer, size_t length)
{
    size_t total_sent = 0;
    while (total_sent < length) {
        size_t temp_sent;
        temp_sent = send(client.socket, buffer, length, 0);

        if (temp_sent == -1) {
            error("Could not send message to client");
            continue;
        }

        total_sent += temp_sent;
    }
}

/* ClientSet functions */

ClientSet* ClientSet_Create(size_t max_clients)
{
    ClientSet* set = malloc(sizeof(ClientSet));
    set->clients = NULL;
    set->size = 0;
    set->max_clients = max_clients;

    return set;
}

void ClientSet_Destroy(ClientSet* set)
{
    int i;
    /* If `set->clients` isn't NULL we need to close
     * all sockets in the set, free `set->clients`
     * and finally `set`.
     */
    if (set->clients != NULL) {
        for (i = 0; i < set->size; i++) {
            printf("Closing socket...\n");
            close(set->clients[i].socket);
        }
        free(set->clients);
        free(set);
    }
}

Client* ClientSet_Add(ClientSet* set, Socket socket)
{
    if (set->size >= set->max_clients) {
        Client temp_client;
        temp_client.socket = socket;
        char* response = "Server full.\n";
        Client_Send(temp_client, response, strlen(response));
        close(socket);
        return NULL;
    }
    int i;
    int index = -1;
    for (i = 0; i < set->size; i++) {
        if (set->clients[i].state == DISCONNECTED) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        set->size++;
        if (set->size == 1) {
            set->clients = malloc(sizeof (Client));
        } else {
            set->clients = realloc(set->clients, sizeof(Client) * set->size);
        }
        index = set->size - 1;
    }


    set->clients[index].socket = socket;
    set->clients[index].state = CONNECTED;
    set->clients[index].last_recv = time(NULL);

    return &(set->clients[index]);
}

size_t ClientSet_Connected(ClientSet* set)
{
    size_t clients = 0;
    size_t i;
    for (i = 0; i < set->size; i++) {
        if (set->clients[i].state == CONNECTED) {
            clients++;
        }
    }

    return clients;
}


size_t ClientSet_MemoryUsage(ClientSet* set)
{
    if (set == NULL) {
        return 0;
    }

    size_t total = 0;
    total += sizeof(*set);
    total += sizeof(Client) * set->size;
    return total;
}
