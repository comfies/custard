#ifndef MONITOR_H
#define MONITOR_H

#include "grid.h"

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int height;
    unsigned int width;

    grid_t *grid;
} monitor_t;

unsigned short initialize_monitors(void);
monitor_t *get_focused_monitor(void);

#endif
