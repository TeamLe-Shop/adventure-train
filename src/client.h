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

#endif /* CLIENT_H */
