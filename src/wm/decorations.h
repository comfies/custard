#pragma once

#include "config.h"
#include "geometry.h"
#include "window.h"

void apply_decoration_to_window_screen_geometry(window_t*, screen_geometry_t*);
unsigned int get_raw_color_value(color_t);
void decorate(window_t*);

void single_border(xcb_window_t, unsigned int, color_t);
void multi_border(xcb_window_t, unsigned int, unsigned int, color_t, color_t,
    unsigned short);

void double_border_transient(xcb_pixmap_t, xcb_gcontext_t,
    unsigned int, unsigned int, unsigned int, xcb_get_geometry_reply_t*);
void triple_border_transient(xcb_pixmap_t, xcb_gcontext_t,
    unsigned int, unsigned int, unsigned int, xcb_get_geometry_reply_t*);
