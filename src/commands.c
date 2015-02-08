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
#include "client.h"

void Command_Read(Client client, ClientSet* clients, NoticeBoard* board)
{
    int i;
    char temp_buffer[512];
    Client_Send(client, "-----\n", 7);

    if (client.properties.noticeboard_order == OLDEST_FIRST) {
        for (i = 0; i < board->length; i++) {
            memset(temp_buffer, 0, 512);
            snprintf(temp_buffer, 512, "%d - %s: %s", i + 1,
                     board->entries[i].poster,
                     board->entries[i].message);
            Client_Send(client, temp_buffer, 512 - 6);
        }
    } else if (client.properties.noticeboard_order == NEWEST_FIRST) {
        for (i = board->length - 1; i > -1; i--) {
            memset(temp_buffer, 0, 512);
            snprintf(temp_buffer, 512, "%d - %s: %s", i + 1,
                     board->entries[i].poster,
                     board->entries[i].message);
            Client_Send(client, temp_buffer, 512 - 6);
        }
    }
    Client_Send(client, "-----\n", 7);
}

void Command_Post(Client client, ClientSet* clients, NoticeBoard* board,
                  char* buffer)
{
    buffer[5 + 134] = 0;
    NoticeBoard_Add(board, client.properties.nick, buffer + 5);
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
    int i;
    char buf[100];

    char header[100];
    memset(header, 0, 100);

    sprintf(header, "----- (%zu / %zu) -----\n", ClientSet_Connected(clients),
            clients->max_clients);
    Client_Send(client, header, 100);
    memset(header, 0, 100);
    sprintf(header, "%-21s      %-22s %-6s\n", "NICK", "ADDR", "IDLE");
    Client_Send(client, header, 100);
    for (i = 0; i < clients->size; i++) {
        memset(buf, 0, 100);

        if (clients->clients[i].state == DISCONNECTED) {
            continue;
        }
        char idle[7];
        strtimediff(idle, clients->clients[i].last_recv);
        if (!strcmp(client.properties.nick,
                    clients->clients[i].properties.nick)) {
            sprintf(buf, "%-21s  **  %-22s %s\n",
                clients->clients[i].properties.nick,
                clients->clients[i].ip_string,
                idle);
        } else {
            sprintf(buf, "%-21s      %-22s %s\n",
                clients->clients[i].properties.nick,
                clients->clients[i].ip_string,
                idle);
        }
        Client_Send(client, buf, 100);
    }
}

void Command_Nick(Client* client, ClientSet* clients, char* buffer)
{
    int i;
    buffer[5 + NICK_MAXLEN] = 0;
    char* ptr = buffer + 5;

    while (*ptr != 0) {
        if (isspace(*ptr)) {
            *ptr = 0;
            break;
        }
        ptr++;
    }

    if (*(buffer + 5) == 0) {

        Client_Send(*client, "Nickname cannot be blank.\n", 26);
        return;
    }

    for (i = 0; i < clients->size; i++) {
        if (clients->clients[i].state == DISCONNECTED) {
            continue;
        }

        if (!strcmp(clients->clients[i].properties.nick, buffer + 5)) {
            Client_Send(*client, "Nickname already in use.\n", 25);
            return;
        }
    }

    char response[256];
    sprintf(response, "You have changed your nickname from \"%s\" to \"%s\".\n",
            client->properties.nick, buffer + 5);
    Client_Send(*client, response, strlen(response));
    memset(client->properties.nick, 0, NICK_MAXLEN);
    strcpy(client->properties.nick, buffer + 5);

}

void Command_Debug(Client* client, ClientSet* clients, NoticeBoard* board)
{
    char debug_info[256] = {0};
    sprintf(debug_info, "Client size:      %zu bytes\n", sizeof(Client));
    Client_Send(*client, debug_info, 256);
    memset(debug_info, 0, 256);

    sprintf(debug_info, "size_t size:      %zu bytes\n", sizeof(size_t));
    Client_Send(*client, debug_info, 256);
    memset(debug_info, 0, 256);

    sprintf(debug_info, "Pointer size:     %zu bytes\n", sizeof(Client*));
    Client_Send(*client, debug_info, 256);
    memset(debug_info, 0, 256);

    sprintf(debug_info, "ClientSet memory: %zu bytes\n\n",
            ClientSet_MemoryUsage(clients));
    Client_Send(*client, debug_info, 256);
    memset(debug_info, 0, 256);

    sprintf(debug_info, "NoticeBoard memory: %zu bytes\n", sizeof(*board));
    Client_Send(*client, debug_info, 256);
}

void Command_Help(Client client)
{
    char response[2048] = {0};
    strcat(response, "help:   Show this help dialog\n");
    strcat(response, "read:   Read all messages on the noticeboard\n");
    strcat(response, "post:   Post a message onto the noticeboard\n");
    strcat(response, "time:   Display the host's local time\n");
    strcat(response, "debug:  Show memory usage information\n");
    strcat(response, "nick:   Change nickname\n");
    strcat(response, "online: See who's online\n");
    strcat(response, "motd:   View the Message of the Day\n");
    Client_Send(client, response, 1024);
}
