#include "client.h"

#include <stdlib.h>
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

ClientSet* ClientSet_Create()
{
    ClientSet* set = malloc(sizeof(ClientSet));
    set->clients = NULL;
    set->size = 0;

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
            close(set->clients[i].socket);
        }
        free(set->clients);
        free(set);
    }
}

void ClientSet_Add(ClientSet* set, Socket socket)
{
    int i;
    int index = -1;
    for (i = 0; i < set->size; i++) {
        if (set->clients[i].state == DISCONNECTED) {
            index = i;
        }
    }

    if (index == -1) {
        index = set->size - 1;
    }

    set->size++;
    if (set->size == 1) {
        set->clients = malloc(sizeof (Client));
    } else {
        set->clients = realloc(set->clients, sizeof(Client) * set->size);
    }
    set->clients[i].socket = socket;
    set->clients[i].state = CONNECTED;
}

