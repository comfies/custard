#include "config.h"

short unsigned border_type = 1;
short unsigned border_invert_colors = 0;

unsigned int border_inner_size = 0;
unsigned int border_outer_size = 3;
unsigned int border_total_size = 3;

unsigned int border_focused_color = 0x888888;
unsigned int border_unfocused_color = 0x333333;
unsigned int border_background_color = 0x000000;

unsigned grid_rows = 6;
unsigned grid_columns = 8;

unsigned int grid_gap = 32;
unsigned int grid_margin_top = 0;
unsigned int grid_margin_bottom = 0;
unsigned int grid_margin_left = 0;
unsigned int grid_margin_right = 0;

unsigned int workspaces = 2;

struct NamedGeometry geometries[] = {
    { .name = "display_left", .geometry = {
        .x = 2, .y = 2, .height = 4, .width = 3 } },
    { .name = "display_top_right", .geometry = {
        .x = 5, .y = 2, .height = 2, .width = 3 } },
    { .name = "display_bottom_right", .geometry = {
        .x = 5, .y = 4, .height = 2, .width = 3 } }
};
