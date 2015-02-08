#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <netinet/in.h>

void error(char* message)
{
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
}

char* ipaddr(const SockAddrIn addr)
{
    uint32_t host = ntohl(addr.sin_addr.s_addr);
    uint16_t port = ntohs(addr.sin_port);
    char* result = malloc(22);
    sprintf(result, "%u.%u.%u.%u:%u", host >> 24 & 0xFF, host >> 16 & 0xFF,
             host >> 8 & 0xFF, host >> 0 & 0xFF, port);
    return result;
}

void strtimediff(char* str, time_t tm)
{
    long diff = difftime(time(NULL), tm);
    sprintf(str, "%dm:%02ds", (int)diff / 60, (int)diff % 60);
}
