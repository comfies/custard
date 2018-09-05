#ifndef WINDOW_H
#define WINDOW_H

#include "custard.h"

#include <xcb/xcb.h>

struct Window {
    xcb_window_t id;
    short unsigned int mapped;
    short unsigned int fullscreen;
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    short unsigned int groups;
};

short unsigned int manage_window(xcb_window_t);
short unsigned int unmanage_window(xcb_window_t);
void update_window_borders(void);

void unfocus_window(void);
void focus_on_window(xcb_window_t);
void close_window(xcb_window_t);

void map_window(xcb_window_t);
void unmap_window(xcb_window_t);

void raise_window(xcb_window_t);
void lower_window(xcb_window_t);

void fullscreen(xcb_window_t);
void window(xcb_window_t);

void move_window_to_pixel_coordinate(xcb_window_t, unsigned int, unsigned int);
void move_window_to_grid_coordinate(xcb_window_t, unsigned int, unsigned int);

void resize_window_with_pixels(xcb_window_t, unsigned int, unsigned int);
void resize_window_with_grid_units(xcb_window_t, unsigned int, unsigned int);

void move_window_cardinal(xcb_window_t, cardinal_direction_t);
void expand_window_cardinal(xcb_window_t, cardinal_direction_t);
void contract_window_cardinal(xcb_window_t, cardinal_direction_t);

void border_update(xcb_window_t);
void border_update_single(xcb_window_t);
void border_update_double(xcb_window_t);
void border_update_triple(xcb_window_t);
void border_update_with_graphics_context(xcb_window_t, xcb_rectangle_t*,
    unsigned int);

#endif /* WINDOW_H */
