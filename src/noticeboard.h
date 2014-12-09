#ifndef NOTICEBOARD_H
#define NOTICEBOARD_H

#define NOTICEBOARD_CAPACITY 64
#define NOTICEBOARD_ENTRY_CAPACITY 128

#include <stdlib.h>

struct _NoticeBoard {
    char entries[NOTICEBOARD_CAPACITY][NOTICEBOARD_ENTRY_CAPACITY];
    size_t length;
};

typedef struct _NoticeBoard NoticeBoard;

NoticeBoard* NoticeBoard_Create();
void NoticeBoard_Destroy(NoticeBoard* noticeboard);
void NoticeBoard_Add(NoticeBoard* noticeboard, char* msg);

#endif  /* NOTICEBOARD_H */
