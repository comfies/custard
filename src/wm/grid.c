#include "config.h"
#include "grid.h"

#include "../xcb/connection.h"

/* Calculations */

unsigned int calculate_default_height(monitor_t* monitor) {
    unsigned int rows = get_value_from_key_with_fallback(
        monitor->configuration, "grid.rows")->number;

    if (rows % 2) return 1;

    return 2;
}

unsigned int calculate_default_width(monitor_t* monitor) {
    unsigned int columns = get_value_from_key_with_fallback(
        monitor->configuration, "grid.columns")->number;

    if (columns % 2) return 1;

    return 2;
}

unsigned int calculate_default_x(monitor_t* monitor) {
    unsigned int columns = get_value_from_key_with_fallback(
        monitor->configuration, "grid.columns")->number;

    unsigned int default_value = (columns / 2) - 1;

    if (columns % 2) default_value++;

    return default_value;
}

unsigned int calculate_default_y(monitor_t* monitor) {
    unsigned int rows = get_value_from_key_with_fallback(
        monitor->configuration, "grid.rows")->number;

    unsigned int default_value = (rows / 2) - 1;

    if (rows % 2) default_value++;

    return default_value;
}

float calculate_horizontal_unit_size(monitor_t* monitor) {
    unsigned int total_border_size = 0;

    unsigned int gap_size = get_value_from_key_with_fallback(
        monitor->configuration, "grid.margins")->number;
    unsigned int columns = get_value_from_key_with_fallback(
        monitor->configuration, "grid.columns")->number;
    unsigned int offset_left = get_value_from_key_with_fallback(
        monitor->configuration, "grid.margin.left")->number;
    unsigned int offset_right = get_value_from_key_with_fallback(
        monitor->configuration, "grid.margin.right")->number;

    unsigned int horizontal_border_summation = (total_border_size * 2) *
        columns;
    unsigned int horizontal_offset = offset_left + offset_right;
    unsigned int horizontal_gap_summation = gap_size * (columns + 1);

    unsigned int negative_real_estate = horizontal_offset +
        horizontal_gap_summation + horizontal_border_summation;

    float usable_real_estate = (float)(monitor->geometry->width -
        negative_real_estate);
    float unit_size = usable_real_estate / (float)columns;

    return unit_size;
}

float calculate_vertical_unit_size(monitor_t* monitor) {
    unsigned int total_border_size = 0;

    unsigned int gap_size = get_value_from_key_with_fallback(
        monitor->configuration, "grid.margins")->number;
    unsigned int rows = get_value_from_key_with_fallback(
        monitor->configuration, "grid.rows")->number;
    unsigned int offset_top = get_value_from_key_with_fallback(
        monitor->configuration, "grid.margin.top")->number;
    unsigned int offset_bottom = get_value_from_key_with_fallback(
        monitor->configuration, "grid.margin.bottom")->number;

    unsigned int vertical_border_summation = (total_border_size * 2) * rows;
    unsigned int vertical_offset = offset_top + offset_bottom;
    unsigned int vertical_gap_summation = gap_size * (rows + 1);

    unsigned int negative_real_estate = vertical_offset +
        vertical_gap_summation + vertical_border_summation;

    float usable_real_estate = (float)(monitor->geometry->height -
        negative_real_estate);
    float unit_size = usable_real_estate / (float)rows;

    return unit_size;
}

/* Lower level helpers */

float span_units_over_screen(float unit_size, unsigned int span,
    monitor_t* monitor) {
    unsigned int total_border_size = 0;
    unsigned int gap_size = get_value_from_key_with_fallback(
        monitor->configuration, "grid.margins")->number;

    return (unit_size * span) + (gap_size * (span - 1)) +
        ((total_border_size * 2) * (span - 1));
}

float get_unit_offset(float unit_size, unsigned int offset,
    monitor_t* monitor) {
    unsigned int total_border_size = 0;
    unsigned int gap_size = get_value_from_key_with_fallback(
        monitor->configuration, "grid.margins")->number;

    return (unit_size * offset) + (gap_size * (offset + 1)) +
        ((total_border_size * 2) * offset);
}

/* Abstractions */

float span_width_over_screen(unsigned int span, monitor_t* monitor) {
    float unit_in_pixels = calculate_horizontal_unit_size(monitor);

    return span_units_over_screen(unit_in_pixels, span, monitor);
}

float span_height_over_screen(unsigned int span, monitor_t* monitor) {
    float unit_in_pixels = calculate_vertical_unit_size(monitor);

    return span_units_over_screen(unit_in_pixels, span, monitor);
}

float get_x_offset(unsigned int offset, monitor_t* monitor) {
    float unit_in_pixels = calculate_horizontal_unit_size(monitor);
    float left_offset = (float)get_value_from_key_with_fallback(
        monitor->configuration, "grid.margin.left")->number;

    return get_unit_offset(unit_in_pixels, offset, monitor) + left_offset;
}

float get_y_offset(unsigned int offset, monitor_t* monitor) {
    float unit_in_pixels = calculate_vertical_unit_size(monitor);
    float top_offset = (float)get_value_from_key_with_fallback(
        monitor->configuration, "grid.margin.top")->number;

    return get_unit_offset(unit_in_pixels, offset, monitor) + top_offset;
}
