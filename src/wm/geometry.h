#ifndef GEOMETRY_H
#define GEOMETRY_H

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

screen_geometry_t* get_equivalent_screen_geometry(grid_geometry_t*, monitor_t*);

#endif
