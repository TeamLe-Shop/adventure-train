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
    if (noticeboard->length >= NOTICEBOARD_CAPACITY) {
        // Shift all messages up
        memmove(noticeboard->entries, noticeboard->entries + 1,
                (NOTICEBOARD_CAPACITY - 1) * NOTICEBOARD_ENTRY_CAPACITY);
        // Write to last
        strncpy(noticeboard->entries[NOTICEBOARD_CAPACITY - 1], msg,
                NOTICEBOARD_ENTRY_CAPACITY);
    } else {
        strncpy(noticeboard->entries[noticeboard->length], msg,
            NOTICEBOARD_ENTRY_CAPACITY);
        noticeboard->length++;
    }
}
