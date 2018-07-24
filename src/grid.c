#include "grid.h"

#include "config.h"
#include "window.h"
#include "xcb.h"

unsigned int grid_x_unit_size_in_pixels = 0;
unsigned int grid_y_unit_size_in_pixels = 0;

unsigned int grid_window_default_x = 0;
unsigned int grid_window_default_y = 0;
unsigned int grid_window_default_height = 1;
unsigned int grid_window_default_width = 1;

void
grid_apply_configuration()
{

    grid_x_unit_size_in_pixels = ((screen->width_in_pixels - (
            ((Configuration->border_total_size * 2) *
                Configuration->grid_columns) +
            (Configuration->grid_margin_left +
                Configuration->grid_margin_right) +
            (Configuration->grid_gap * (Configuration->grid_columns + 1))
        )) / Configuration->grid_columns);

    grid_y_unit_size_in_pixels = ((screen->height_in_pixels - (
            ((Configuration->border_total_size * 2) *
                Configuration->grid_rows) +
            (Configuration->grid_margin_top +
                Configuration->grid_margin_bottom) +
            (Configuration->grid_gap * (Configuration->grid_rows + 1))
        )) / Configuration->grid_rows);

    if (Configuration->grid_columns % 2) {
        grid_window_default_x = (Configuration->grid_columns / 2) + 1;
    } else {
        grid_window_default_x = Configuration->grid_columns / 2;
        grid_window_default_width = 2;
    }

    if (Configuration->grid_rows % 2) {
        grid_window_default_y = (Configuration->grid_rows / 2) + 1;
    } else {
        grid_window_default_y = Configuration->grid_rows / 2;
        grid_window_default_height = 2;
    }

}

unsigned int
grid_get_span(unsigned int unit_size_in_pixels, unsigned int amount)
{
    return (unit_size_in_pixels * amount) + (
        Configuration->grid_gap * (amount - 1)
    ) + (
        (Configuration->border_total_size * 2) * (amount - 1)
    );
}

unsigned
grid_get_span_x(unsigned int amount)
{
    return grid_get_span(grid_x_unit_size_in_pixels, amount);
}

unsigned
grid_get_span_y(unsigned int amount)
{
    return grid_get_span(grid_y_unit_size_in_pixels, amount);
}

unsigned
grid_get_offset(unsigned int unit_size_in_pixels, unsigned int amount)
{
    return (
        Configuration->grid_gap * (amount)
    ) + (unit_size_in_pixels * (amount - 1)) + (
        (Configuration->border_total_size * 2) * (amount - 1)
    );
}

unsigned
grid_get_offset_x(unsigned int amount)
{
    return grid_get_offset(grid_x_unit_size_in_pixels, amount);
}

unsigned
grid_get_offset_y(unsigned int amount)
{
    return grid_get_offset(grid_y_unit_size_in_pixels, amount);
}
