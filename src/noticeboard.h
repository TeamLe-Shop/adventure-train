#ifndef NOTICEBOARD_H
#define NOTICEBOARD_H

#define NOTICEBOARD_MAX_LEN 64
#define NOTICEBOARD_MAX_MSG_LEN 128

#include <stdlib.h>

struct _NoticeBoard {
    char board[NOTICEBOARD_MAX_LEN][NOTICEBOARD_MAX_MSG_LEN];
    size_t size;
};

typedef struct _NoticeBoard NoticeBoard;

NoticeBoard* NoticeBoard_Create();
void NoticeBoard_Destroy(NoticeBoard* noticeboard);
void NoticeBoard_Add(NoticeBoard* noticeboard, char* msg);

#endif  /* NOTICEBOARD_H */
