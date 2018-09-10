#include "config.h"

short unsigned border_type = 0;
short unsigned border_invert_colors = 0;

unsigned int border_inner_size = 0;
unsigned int border_outer_size = 0;
unsigned int border_total_size = 0;

unsigned int border_focused_color = 0xFFFFFFff;
unsigned int border_unfocused_color = 0x676767ff;
unsigned int border_background_color = 0x000000ff;

unsigned grid_rows = 2;
unsigned grid_columns = 3;

unsigned int grid_gap = 0;
unsigned int grid_margin_top = 0;
unsigned int grid_margin_bottom = 0;
unsigned int grid_margin_left = 0;
unsigned int grid_margin_right = 0;

unsigned int workspaces = 2;

/* when modifying the geometries array make sure the NULL stays at the
   end of the array because that's how custard knows to stop */
struct NamedGeometry geometries[] = {
    { .name = "top_left", .geometry = {
        .x = 1, .y = 1, .height = 1, .width = 1 } },
    { .name = "top_mid", .geometry = {
        .x = 2, .y = 1, .height = 1, .width = 1 } },
    { .name = "top_right", .geometry = {
        .x = 3, .y = 1, .height = 1, .width = 1 } },

    { .name = "bottom_left", .geometry = {
        .x = 1, .y = 2, .height = 1, .width = 1 } },
    { .name = "bottom_mid", .geometry = {
        .x = 2, .y = 2, .height = 1, .width = 1 } },
    { .name = "bottom_right", .geometry = {
        .x = 3, .y = 2, .height = 1, .width = 1 } },
    { 0 } /* no touch */
};
