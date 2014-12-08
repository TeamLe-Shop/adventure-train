#ifndef UTIL_H
#define UTIL_H

#include <string.h>

#include "mysocket.h"

#define starts(a, b) !memcmp(a, b, strlen(b))

/* Prints "message: error" where
 * message = `message` and
 * error = strerror(errno).
 */
void error(char* message);

/* Returns addr formated in i.i.i.i:port */
char* ipaddr(const SockAddrIn addr);

#endif /* UTIL_H */
