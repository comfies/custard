unsigned int grid_x_unit_size_in_pixels = 0;
unsigned int grid_y_unit_size_in_pixels = 0;

unsigned int grid_window_default_x = 0;
unsigned int grid_window_default_y = 0;
unsigned int grid_window_default_height = 1;
unsigned int grid_window_default_width = 1;

void grid_apply_configuration(void);

unsigned int grid_get_span(unsigned int, unsigned int);
unsigned int grid_get_span_x(unsigned int);
unsigned int grid_get_span_y(unsigned int);

unsigned int grid_get_offset(unsigned int, unsigned int);
unsigned int grid_get_offset_x(unsigned int);
unsigned int grid_get_offset_y(unsigned int);
