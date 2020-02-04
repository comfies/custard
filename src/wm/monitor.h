#ifndef MONITOR_H
#define MONITOR_H

#include "geometry.h"

#include "../vector.h"

extern vector_t* monitors;

struct monitor {
    char* name;
    screen_geometry_t* geometry;
    vector_t* configuration;
};

void setup_monitors(void);
monitor_t* monitor_from_name(char*);
monitor_t* monitor_with_cursor_residence(void);

#endif
