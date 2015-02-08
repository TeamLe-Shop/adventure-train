#ifndef UTIL_H
#define UTIL_H

#include <string.h>

#include "mysocket.h"

#define starts(a, b) !memcmp(a, b, strlen(b))

#define IP_MAX_LEN 22

/* Prints "message: error" where
 * message = `message` and
 * error = strerror(errno).
 */
void error(char* message);

/* Returns addr formated in i.i.i.i:port */
char* ipaddr(const SockAddrIn addr);

/* Stores the time difference between `tm` and time(NULL) in the format:
 *   00m:00s
 *  in `str`.
 */
void strtimediff(char* str, time_t tm);

#endif /* UTIL_H */
