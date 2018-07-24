#ifndef GRID_H
#define GRID_H

extern unsigned int grid_x_unit_size_in_pixels;
extern unsigned int grid_y_unit_size_in_pixels;

extern unsigned int grid_window_default_x;
extern unsigned int grid_window_default_y;
extern unsigned int grid_window_default_height;
extern unsigned int grid_window_default_width;

void grid_apply_configuration(void);

unsigned int grid_get_span(unsigned int, unsigned int);
unsigned int grid_get_span_x(unsigned int);
unsigned int grid_get_span_y(unsigned int);

unsigned int grid_get_offset(unsigned int, unsigned int);
unsigned int grid_get_offset_x(unsigned int);
unsigned int grid_get_offset_y(unsigned int);

#endif /* GRID_H */
