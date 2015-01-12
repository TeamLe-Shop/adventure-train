#include "noticeboard.h"

#include <string.h>

#include <stdio.h>

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

void NoticeBoard_Add(NoticeBoard* noticeboard, char* poster, char* msg)
{
    msg[NOTICEBOARD_ENTRY_CAPACITY] = 0;
    if (noticeboard->length >= NOTICEBOARD_CAPACITY) {
        /* Shift all messages up */
        memmove(&noticeboard->entries, &(noticeboard->entries[1]),
            sizeof(noticeboard->entries) - sizeof(Entry));
        /* Write to last */
        strcpy(noticeboard->entries[NOTICEBOARD_CAPACITY - 1].message, msg);
        strcpy(noticeboard->entries[NOTICEBOARD_CAPACITY - 1].poster, poster);
    } else {
        strcpy(noticeboard->entries[noticeboard->length].message, msg);
        strcpy(noticeboard->entries[noticeboard->length].poster, poster);
        noticeboard->length++;
    }
}
