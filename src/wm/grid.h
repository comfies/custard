#pragma once

#include "monitor.h"

unsigned int calculate_default_height(monitor_t*);
unsigned int calculate_default_width(monitor_t*);
unsigned int calculate_default_x(monitor_t*);
unsigned int calculate_default_y(monitor_t*);

float calculate_horizontal_unit_size(monitor_t*);
float calculate_vertical_unit_size(monitor_t*);

float span_units_over_screen(float, unsigned int, monitor_t*);
float get_unit_offset(float, unsigned int, monitor_t*);

float span_width_over_screen(unsigned int, monitor_t*);
float span_height_over_screen(unsigned int, monitor_t*);

float get_x_offset(unsigned int, monitor_t*);
float get_y_offset(unsigned int, monitor_t*);
