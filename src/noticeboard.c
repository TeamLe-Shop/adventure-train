#include "noticeboard.h"

#include <string.h>

NoticeBoard* NoticeBoard_Create()
{
    NoticeBoard* noticeboard = malloc(sizeof(NoticeBoard));
    memset(noticeboard, 0, sizeof(NoticeBoard));
    return noticeboard;
}

void NoticeBoard_Destroy(NoticeBoard* noticeboard)
{
    free(noticeboard);
}

void NoticeBoard_Add(NoticeBoard* noticeboard, char* msg)
{
    int i;

    /* This is really inefficient, isn't it?
     * How else will I do it?
     */

    strcpy(noticeboard->board[noticeboard->size], msg);

    if (noticeboard->size < NOTICEBOARD_MAX_LEN) {
        noticeboard->size++;
    } else {
        for (i = 0; i < noticeboard->size - 1; i++) {
            memset(noticeboard->board[i], 0, 128);
            strcpy(noticeboard->board[i], noticeboard->board[i + 1]);
        }
    }
}
