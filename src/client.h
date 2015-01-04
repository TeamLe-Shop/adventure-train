#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

#include "mysocket.h"

#define NICK_MAXLEN 20

enum _State {
    DISCONNECTED, CONNECTED
};

typedef enum _State State;

struct _ClientProperties {
    char nick[NICK_MAXLEN + 1];
    bool show_joins_exits;
};

typedef struct _ClientProperties ClientProperties;

struct _Client {
    Socket socket;
    State state;
    ClientProperties properties;
};

typedef struct _Client Client;

/* Client functions */

void Client_Send(Client client, char* buffer, size_t length);

struct _ClientSet {
    Client* clients;
    size_t size;
    size_t max_clients;
};

typedef struct _ClientSet ClientSet;

/* ClientSet functions */

/* This allocates memory for and returns a ClientSet. */
ClientSet* ClientSet_Create(size_t max_clients);

/* This closes all sockets contained in `set->client`,
 * frees `set->clients` and `set`.
 */
void ClientSet_Destroy(ClientSet* set);

/* Add a client to set->client, whose socket will be `socket`. */
Client* ClientSet_Add(ClientSet* set, Socket socket);

/* Return the amount of clients connected in `set`. */
size_t ClientSet_Connected(ClientSet* set);

/* Returns the amount of memory used by `set`. */
size_t ClientSet_MemoryUsage(ClientSet* set);

#endif /* CLIENT_H */
