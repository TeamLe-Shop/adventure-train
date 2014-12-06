#ifndef CLIENT_H
#define CLIENT_H

#include "mysocket.h"

enum _State {
    PENDING,
    CONNECTED,
    DISCONNECTED
};

typedef enum _State State;

struct _Client {
    Socket socket;
    State state;
};

typedef struct _Client Client;

struct _ClientSet {
    Client* clients;
    size_t size;
};

typedef struct _ClientSet ClientSet;

/* ClientSet functions */
ClientSet* ClientSet_Create();
void ClientSet_Destroy(ClientSet* set);
void ClientSet_Add(ClientSet* set, Socket socket);

#endif /* CLIENT_H */
