#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>
#include <time.h>

#include "mysocket.h"

#define NICK_MAXLEN 20

enum _State;

typedef enum _State State;

struct _ClientProperties;

typedef struct _ClientProperties ClientProperties;

struct _Client;

typedef struct _Client Client;

enum _NBOrder;

typedef enum _NBOrder NBOrder;

/* The state of a client. */
enum _State {
    DISCONNECTED, CONNECTED
};

/* How to order the noticeboard for certain users.
 *
 * LATEST_FIRST: "Bottom to top", show latest ones first.
 * OLDEST_FIRST: "Top to bottom", show oldest ones first.
 */
enum _NBOrder {
    NEWEST_FIRST, OLDEST_FIRST
};

struct _ClientProperties {
    char nick[NICK_MAXLEN + 1];
    NBOrder noticeboard_order;
};

struct _Client {
    Socket socket;
    State state;
    time_t last_recv;
    char* ip_string;
    bool destroyed;
    ClientProperties properties;
};

struct _ClientSet {
    Client* clients;
    size_t size;
    size_t max_clients;
};

/* Client functions */

/* Send `length` bytes from string `buffer` to `client`. */
void Client_Send(Client client, char* buffer, size_t length);

/* Destroy `client` by freeing allocated memory and closing socket. */
void Client_Destroy(Client* client);

struct _ClientSet;

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
