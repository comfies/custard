#ifndef GRID_H
#define GRID_H


typedef struct {
    unsigned int default_x;
    unsigned int default_y;
    unsigned int default_width;
    unsigned int default_height;

    float x_unit_size_in_pixels;
    float y_unit_size_in_pixels;
} grid_t;

#include "monitor.h"

/*
float grid_x_unit_size_in_pixels;
float grid_y_unit_size_in_pixels;

extern unsigned int grid_window_default_x;
extern unsigned int grid_window_default_y;
extern unsigned int grid_window_default_height;
extern unsigned int grid_window_default_width;
*/
void apply_configuration_to_monitor_grid(monitor_t *);

float grid_get_span(float, unsigned int);
float grid_get_offset(float, unsigned int);

float grid_get_span_x(unsigned int);
float grid_get_span_y(unsigned int);

float grid_get_x_offset(unsigned int);
float grid_get_y_offset(unsigned int);

#endif /* GRID_H */
