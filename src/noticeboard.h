#ifndef NOTICEBOARD_H
#define NOTICEBOARD_H

#define NOTICEBOARD_CAPACITY 32
#define NOTICEBOARD_ENTRY_CAPACITY 128

#include <stdlib.h>

#include "client.h"

struct _Entry {
    char message[NOTICEBOARD_ENTRY_CAPACITY];
    char poster[20];
};

typedef struct _Entry Entry;

struct _NoticeBoard {
    Entry entries[NOTICEBOARD_CAPACITY];
    size_t length;
};

typedef struct _NoticeBoard NoticeBoard;

NoticeBoard* NoticeBoard_Create();
void NoticeBoard_Destroy(NoticeBoard* noticeboard);
void NoticeBoard_Add(NoticeBoard* noticeboard, char* poster, char* msg);

#endif  /* NOTICEBOARD_H */
