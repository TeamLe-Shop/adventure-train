#include "commands.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <ctype.h>

#include "util.h"
#include "mysocket.h"

void Command_Read(Client client, ClientSet* clients, NoticeBoard* board)
{
    char temp_buffer[512];
    Client_Send(client, "-----\n", 7);

    for (int i = 0; i < board->length; i++) {
        memset(temp_buffer, 0, 512);
        snprintf(temp_buffer, 512, "%d: %s", i + 1, board->entries[i]);
        Client_Send(client, temp_buffer, 512 - 6);
    }
    Client_Send(client, "-----\n", 7);
}

void Command_Post(Client client, ClientSet* clients, NoticeBoard* board,
                  char* buffer)
{
    buffer[5 + 134] = 0;
    NoticeBoard_Add(board, buffer + 5);
    char* response = "Your post has been posted on the board.\n";
    Client_Send(client, response, strlen(response));
}

void Command_Time(Client client)
{
    time_t current_time;
    current_time = time(NULL);
    /* 0'd out char[] */
    char time_str[40] = {0};
    /* "Host's time: <local time format>" */
    snprintf(time_str, 40, "Host's time: %s", ctime(&current_time));
    Client_Send(client, time_str, 40);
}

void Command_Online(Client client, ClientSet* clients)
{
    int buf_len = 65;
    char buf[buf_len];

    buf[buf_len - 1] = 0;
    char header[100];
    memset(header, 0, 100);
    sprintf(header, "----- (%zu / %zu) -----\n", ClientSet_Connected(clients),
            clients->max_clients);
    Client_Send(client, header, 100);
    for (int i = 0; i < clients->size; i++) {
        memset(buf, 0, buf_len);
        if (clients->clients[i].state == DISCONNECTED) {
            continue;
        }
        SockAddr peer_address;
        socklen_t addrlen = sizeof(peer_address);
        int has_error = getpeername(clients->clients[i].socket, &peer_address,
                            &addrlen);
        if (has_error == -1) {
            error("Error getting peer name");
        }
        SockAddrIn *addr_in = (struct sockaddr_in *)&peer_address;
        sprintf(buf, "%-21s  [%s]\n",
                clients->clients[i].properties.nick,
                ipaddr(*addr_in));
        Client_Send(client, buf, buf_len);
    }
}

void Command_Nick(Client* client, ClientSet* clients, char* buffer)
{
    buffer[5 + NICK_MAXLEN] = 0;
    char* ptr = buffer + 5;
    while (*ptr != 0) {
        if (isspace(*ptr)) {
            *ptr = ' ';
        }
        ptr++;
    }

    char response[256];
    sprintf(response, "You have changed your nickname from \"%s\" to \"%s\".\n",
            client->properties.nick, buffer + 5);
    Client_Send(*client, response, strlen(response));
    memset(client->properties.nick, 0, NICK_MAXLEN);
    strcpy(client->properties.nick, buffer + 5);

}

void Command_Debug(Client* client, ClientSet* clients)
{
    char debug_info[100];
    memset(debug_info, 0, 100);
    size_t memory = sizeof(Client) * clients->size + sizeof(size_t) * 2;
    sprintf(debug_info, "ClientSet memory: %zu\n", memory);
    Client_Send(*client, debug_info, 100);
}

