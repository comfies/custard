#ifndef CUSTARD_H
#define CUSTARD_H

#include <xcb/xcb.h>

#include "vector.h"

extern unsigned short window_manager_is_running;

extern vector_t *managed_windows;
extern vector_t *named_geometries;
extern vector_t *window_rules;
extern vector_t *workspaces;

extern xcb_window_t focused_window;
extern unsigned int focused_workspace;

extern unsigned short debug_mode;
extern char *rc_file;

extern unsigned short border_type;
extern unsigned short border_invert_colors;

extern unsigned int border_inner_size;
extern unsigned int border_outer_size;
extern unsigned int border_total_size;

extern unsigned int border_focused_color;
extern unsigned int border_unfocused_color;
extern unsigned int border_background_color;

extern unsigned int grid_rows;
extern unsigned int grid_columns;

extern unsigned int grid_gap;
extern unsigned int grid_offset_top;
extern unsigned int grid_offset_bottom;
extern unsigned int grid_offset_left;
extern unsigned int grid_offset_right;

extern unsigned int number_of_workspaces;

int start_custard(void);
void stop_custard(void);

void signal_handler(int);

void _debug_output(const char*, const char*, const int, char*, ...);
#define debug_output(...) \
    _debug_output(__FILE__, __func__, __LINE__, __VA_ARGS__)

#endif /* CUSTARD_H */
