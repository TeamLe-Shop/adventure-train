#include "client.h"

#include <stdlib.h>

ClientSet* ClientSet_Create()
{
    ClientSet* set = malloc(sizeof(ClientSet));
    set->clients = NULL;
    set->size = 0;

    return set;
}

void ClientSet_Destroy(ClientSet* set)
{
    if (set->clients) {
        free(set->clients);
    }
    free(set);
}

void ClientSet_Add(ClientSet* set, Socket socket)
{
    set->size++;
    if (set->size == 1) {
        set->clients = malloc(sizeof (Client));
    } else {
        set->clients = realloc(set->clients, sizeof(Client) * set->size);
    }
}
