#include <stdlib.h>
#include <string.h>

#include "geometry.h"
#include "grid.h"

labeled_grid_geometry_t *create_labeled_geometry(char *label, unsigned int x,
    unsigned int y, unsigned int height, unsigned int width) {
    labeled_grid_geometry_t *geometry = (labeled_grid_geometry_t*)malloc(
        sizeof(labeled_grid_geometry_t));
    geometry->label = (char*)malloc(sizeof(char));
    geometry->geometry = (grid_geometry_t*)malloc(sizeof(grid_geometry_t));

    strcpy(geometry->label, label);
    geometry->geometry->x = x;
    geometry->geometry->y = y;
    geometry->geometry->height = height;
    geometry->geometry->width = width;

    return geometry;
}

screen_geometry_t *create_screen_geometry(unsigned int x, unsigned int y,
    unsigned int height, unsigned int width) {
    screen_geometry_t *geometry = (screen_geometry_t*)malloc(
        sizeof(screen_geometry_t));

    geometry->x = x;
    geometry->y = y;
    geometry->height = height;
    geometry->width = width;

    return geometry;
}

screen_geometry_t *get_equivalent_screen_geometry(grid_geometry_t *geometry,
    monitor_t *monitor) {
    screen_geometry_t *screen_geometry = create_screen_geometry(
        get_x_offset(geometry->x, monitor),
        get_y_offset(geometry->y, monitor),
        span_height_over_screen(geometry->height, monitor),
        span_width_over_screen(geometry->width, monitor)
    );

    return screen_geometry;
}

grid_geometry_t *get_geometry_from_monitor(monitor_t *monitor, char *label) {
    if (!monitor->geometries)
        return NULL;

    labeled_grid_geometry_t *labeled_geometry;
    while ((labeled_geometry = vector_iterator(monitor->geometries))) {
        if (!strcmp(labeled_geometry->label, label)) {
            reset_vector_iterator(monitor->geometries);
            return labeled_geometry->geometry;
        }
    }

    return NULL;
}
