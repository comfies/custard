#ifndef WM_WINDOW_H
#define WM_WINDOW_H

#include <xcb/xcb.h>

#include "geometry.h"
#include "rules.h"

extern vector_t* windows;
extern xcb_window_t focused_window;

typedef struct {
    xcb_window_t id;
    xcb_window_t parent;
    grid_geometry_t* geometry;
    rule_t* rule;
} window_t;

unsigned short window_should_be_managed(xcb_window_t);
unsigned short window_is_managed(xcb_window_t);
window_t* get_window_by_id(xcb_window_t);

window_t* manage_window(xcb_window_t);
void unmanage_window(xcb_window_t);

void set_window_geometry(window_t*, grid_geometry_t*);
void focus_on_window(window_t*);
void border_update(window_t*);

#endif
