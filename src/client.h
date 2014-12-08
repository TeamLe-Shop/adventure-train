#ifndef CLIENT_H
#define CLIENT_H

#include "mysocket.h"

enum _State {
    CONNECTED,
    DISCONNECTED
};

typedef enum _State State;

struct _Client {
    Socket socket;
    State state;
};

typedef struct _Client Client;

/* Client functions */

void Client_Send(Client client, char* buffer, size_t length);

struct _ClientSet {
    Client* clients;
    size_t size;
};

typedef struct _ClientSet ClientSet;

/* ClientSet functions */

/* This allocates memory for and returns a ClientSet. */
ClientSet* ClientSet_Create();

/* This closes all sockets contained in `set->client`,
 * frees `set->clients` and `set`.
 */
void ClientSet_Destroy(ClientSet* set);

/* Add a client to set->client, whose socket will be `socket`. */
void ClientSet_Add(ClientSet* set, Socket socket);

#endif /* CLIENT_H */
