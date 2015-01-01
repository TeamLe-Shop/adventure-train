#ifndef COMMANDS_H
#define COMMANDS_H

#include "client.h"
#include "noticeboard.h"

void Command_Read(Client client, ClientSet* clients, NoticeBoard* board);
void Command_Post(Client client, ClientSet* clients, NoticeBoard* board,
                  char* buffer);
void Command_Time(Client client);
void Command_Online(Client client, ClientSet* clients);
void Command_Nick(Client* client, ClientSet* clients, char* buffer);
void Command_Debug(Client* client, ClientSet* clients);

#endif /* COMMANDS_H */
