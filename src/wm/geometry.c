#include <stdlib.h>

#include "geometry.h"
#include "grid.h"

screen_geometry_t* get_equivalent_screen_geometry(grid_geometry_t* geometry,
    monitor_t* monitor) {
    screen_geometry_t* screen_geometry = (screen_geometry_t*)malloc(
        sizeof(screen_geometry_t));

    screen_geometry->x = get_x_offset(geometry->x, monitor);
    screen_geometry->y = get_y_offset(geometry->y, monitor);
    screen_geometry->width = span_width_over_screen(geometry->width, monitor);
    screen_geometry->height = span_height_over_screen(geometry->height,
        monitor);

    return screen_geometry;
}
