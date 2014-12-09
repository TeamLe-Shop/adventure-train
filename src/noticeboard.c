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

    strncpy(noticeboard->entries[noticeboard->length], msg,
            NOTICEBOARD_ENTRY_CAPACITY);

    if (noticeboard->length < NOTICEBOARD_CAPACITY) {
        noticeboard->length++;
    } else {
        for (i = 0; i < noticeboard->length - 1; i++) {
            memset(noticeboard->entries[i], 0, NOTICEBOARD_ENTRY_CAPACITY);
            strncpy(noticeboard->entries[i], noticeboard->entries[i + 1],
                    NOTICEBOARD_ENTRY_CAPACITY);
        }
    }
}
