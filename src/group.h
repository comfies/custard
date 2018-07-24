#ifndef GROUP_H
#define GROUP_H

#include "ipc.h"
#include "window.h"

#include <stdlib.h>

typedef enum {
    UNMAPPED,
    MAPPED,
    FOCUSED
} group_state_t;

extern short unsigned int groups;
extern unsigned int focused_group;

group_state_t get_group_state(unsigned int);

unsigned short window_is_in_group(xcb_window_t, unsigned int);

void attach_window_to_group(xcb_window_t, unsigned int);
void detach_window_from_group(xcb_window_t, unsigned int);

void focus_group(unsigned int);
void map_group(unsigned int);
void unmap_group(unsigned int);

#endif /* GROUP_H */
