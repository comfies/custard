#pragma once

#include "../vector.h"

typedef struct monitor monitor_t;

typedef struct {
    float x;
    float y;
    float height;
    float width;
} screen_geometry_t;

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int height;
    unsigned int width;
} grid_geometry_t;

typedef struct {
    grid_geometry_t *geometry;
    char *label;
} labeled_grid_geometry_t;

labeled_grid_geometry_t *create_labeled_geometry(char*, unsigned int,
    unsigned int, unsigned int, unsigned int);
void add_labeled_geometry(labeled_grid_geometry_t*);
screen_geometry_t *create_screen_geometry(unsigned int, unsigned int,
    unsigned int, unsigned int);
screen_geometry_t *get_equivalent_screen_geometry(grid_geometry_t*, monitor_t*);
grid_geometry_t *get_geometry_from_monitor(monitor_t*, char*);
