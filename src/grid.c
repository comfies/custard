
#include "custard.h"
#include "grid.h"
#include "monitor.h"
#include "xcb.h"

float grid_x_unit_size_in_pixels = 0;
float grid_y_unit_size_in_pixels = 0;

unsigned int grid_window_default_x = 0;
unsigned int grid_window_default_y = 0;
unsigned int grid_window_default_height = 1;
unsigned int grid_window_default_width = 1;

void apply_configuration_to_monitor_grid(monitor_t *monitor) {

    if (!monitor)
        return;

    grid_t *grid = monitor->grid;

    unsigned int horizontal_border_sum = (border_total_size * 2) *
        grid_columns;
    unsigned int horizontal_offset_sum = grid_offset_left + grid_offset_right;
    unsigned int horizontal_gap_sum = grid_gap * (grid_columns + 1);

    float usable_x_pixels = (float)(monitor->width - (
                horizontal_border_sum + horizontal_offset_sum +
                    horizontal_gap_sum));

//    grid_x_unit_size_in_pixels = usable_x_pixels / (float)grid_columns;
    grid->x_unit_size_in_pixels = usable_x_pixels / (float)grid_columns;

    unsigned int vertical_border_sum = (border_total_size * 2) * grid_rows;
    unsigned int vertical_offset_sum = grid_offset_top + grid_offset_bottom;
    unsigned int vertical_gap_sum = grid_gap * (grid_rows + 1);

    float usable_y_pixels = (float)(monitor->height - (
                vertical_border_sum + vertical_offset_sum + vertical_gap_sum));

//    grid_y_unit_size_in_pixels = usable_y_pixels / (float)grid_rows;
    grid->y_unit_size_in_pixels = usable_y_pixels / (float)grid_rows;

    grid->default_x = (grid_columns / 2) - 1;

    if (!(grid_columns % 2)) {
        grid->default_width = 2;
    } else {
        grid->default_x++;
        grid->default_width--;
    }

    grid->default_y = (grid_rows / 2) - 1;

    if (!(grid_rows % 2)) {
        grid->default_height = 2;
    } else {
        grid->default_y++;
        grid->default_height--;
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
    monitor_t *focused_monitor = get_focused_monitor();
    return grid_get_span(focused_monitor->grid->x_unit_size_in_pixels, amount);
}

float grid_get_span_y(unsigned int amount) {
    monitor_t *focused_monitor = get_focused_monitor();
    return grid_get_span(focused_monitor->grid->y_unit_size_in_pixels, amount);
}

float grid_get_x_offset(unsigned int amount) {
    monitor_t *focused_monitor = get_focused_monitor();
    return grid_get_offset(
            focused_monitor->grid->x_unit_size_in_pixels, amount) +
        focused_monitor->x;
}

float grid_get_y_offset(unsigned int amount) {
    monitor_t *focused_monitor = get_focused_monitor();
    return grid_get_offset(
            focused_monitor->grid->y_unit_size_in_pixels, amount) +
        focused_monitor->y;
}
