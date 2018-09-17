#ifndef CUSTARD_H
#define CUSTARD_H

#include <xcb/xcb.h>

#define event_mask XCB_CW_EVENT_MASK

typedef struct Window Window;

extern unsigned short debug;
extern unsigned short wm_running;
extern const char *config_path;

extern int xcb_file_descriptor;

struct LinkedListElement {
    void *next;
    void *data;
};

extern struct LinkedListElement *window_list_head;
extern Window *focused_window;

typedef enum {
    NORTH,
    SOUTH,
    WEST,
    EAST,
    NORTHWEST,
    NORTHEAST,
    SOUTHWEST,
    SOUTHEAST,
    ALL
} cardinal_direction_t;

unsigned int window_list_get_size(void);
unsigned short window_list_append_window(Window*);
unsigned short window_list_remove_window(xcb_window_t);
Window *window_list_get_window(xcb_window_t);

void _debug(const char*, const char*, const int, char*, ...);
//#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define debug_output(...) _debug(__FILE__, __func__, __LINE__, __VA_ARGS__)

int start_custard(void);
void stop_custard(void);

void focus_next_window(void);

#endif /* CUSTARD_H */
