
#include "custard.h"
#include "grid.h"
#include "xcb.h"

float grid_x_unit_size_in_pixels = 0;
float grid_y_unit_size_in_pixels = 0;

unsigned int grid_window_default_x = 0;
unsigned int grid_window_default_y = 0;
unsigned int grid_window_default_height = 1;
unsigned int grid_window_default_width = 1;

void apply_configuration_to_grid() {
    grid_x_unit_size_in_pixels = (float)((screen->width_in_pixels - (
        ((border_total_size * 2) * grid_columns) +
        (grid_offset_left + grid_offset_right) +
        (grid_gap * (grid_columns + 1)))) / (float)grid_columns);

    grid_y_unit_size_in_pixels = (float)((screen->height_in_pixels - (
        ((border_total_size * 2) * grid_rows) +
        (grid_offset_top + grid_offset_bottom) +
        (grid_gap * (grid_rows + 1)))) / (float)grid_rows);

    grid_window_default_x = (grid_columns / 2) - 1;

    if (!(grid_columns % 2)) {
        grid_window_default_width = 2;
    } else {
        grid_window_default_x++;
        grid_window_default_width--;
    }

    grid_window_default_y = (grid_rows / 2) - 1;

    if (!(grid_rows % 2)) {
        grid_window_default_height = 2;
    } else {
        grid_window_default_y++;
        grid_window_default_height--;
    }

}

float grid_get_span(float unit_size, unsigned int amount) {
    return (unit_size * amount) + (grid_gap * (amount - 1)) + (
        (border_total_size * 2) * (amount - 1));
}

float grid_get_offset(float unit_size, unsigned int amount) {
    return (grid_gap * (amount + 1)) + (unit_size * amount) + (
        (border_total_size * 2) * amount);
}

float grid_get_span_x(unsigned int amount) {
    return grid_get_span(grid_x_unit_size_in_pixels, amount);
}

float grid_get_span_y(unsigned int amount) {
    return grid_get_span(grid_y_unit_size_in_pixels, amount);
}

float grid_get_x_offset(unsigned int amount) {
    return grid_get_offset(grid_x_unit_size_in_pixels, amount);
}

float grid_get_y_offset(unsigned int amount) {
    return grid_get_offset(grid_y_unit_size_in_pixels, amount);
}
