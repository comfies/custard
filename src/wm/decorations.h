#ifndef DECORATIONS_H
#define DECORATIONS_H

#include "geometry.h"
#include "window.h"

void decorate(window_t*);
unsigned int determine_border_size(window_t*);
void apply_decoration_to_window_screen_geometry(window_t*, screen_geometry_t*);
void decorate_with_one_border(window_t*);
void decorate_with_multiple_borders(window_t*, unsigned int);

#endif
