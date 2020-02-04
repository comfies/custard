#ifndef WM_WINDOW_H
#define WM_WINDOW_H

#include <xcb/xcb.h>

#include "geometry.h"

typedef struct {
    xcb_window_t id;
    xcb_window_t parent;
    grid_geometry_t* geometry;
} window_t;

unsigned short window_should_be_managed(xcb_window_t);
void manage_window(xcb_window_t);
void unmanage_window(xcb_window_t);

void set_window_geometry(window_t*, grid_geometry_t*);
void border_update(window_t*);

#endif
