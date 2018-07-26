#include "config.h"

#include "custard.h"
#include "grid.h"
#include "window.h"
#include "xcb.h"

struct Config *Configuration;

void
apply_config_defaults()
{
    Configuration->border_type = 0;
    Configuration->border_invert_colors = 0;
    Configuration->border_inner_size = 0;
    Configuration->border_outer_size = 0;
    Configuration->border_total_size = 0;
    Configuration->border_focused_color = 0xFFFFFFFF;
    Configuration->border_unfocused_color = 0xFF676767;
    Configuration->border_background_color = 0xFF000000;
    Configuration->grid_rows = 2;
    Configuration->grid_columns = 3;
    Configuration->grid_gap = 0;
    Configuration->grid_margin_top = 0;
    Configuration->grid_margin_bottom = 0;
    Configuration->grid_margin_left = 0;
    Configuration->grid_margin_right = 0;
    Configuration->groups = 2;
}

void
apply_config()
{
    if (Configuration->border_type == 0) {
        Configuration->border_total_size = 0;
    } else if (Configuration->border_type == 1) {
        Configuration->border_total_size = Configuration->border_outer_size;
    } else if (Configuration->border_type == 2) {
        Configuration->border_total_size = Configuration->border_inner_size +
            Configuration->border_outer_size;
    } else if (Configuration->border_type == 3) {
        Configuration->border_total_size = Configuration->border_inner_size + (
            Configuration->border_outer_size * 2);
    }

    grid_apply_configuration();
    /* TODO: if windows are outside of newly assigned grid boundaries they should be moved
        to fit within the new ones, and resized if need be. */

    /* TODO: if the number of groups is less than the previous amount, move all windows
        from non-accessible groups to the last group in the stack */

    struct WindowLinkedListElement *element = window_list_head;
    Window *window = NULL;

    while (element) {
        window = element->window;
        move_window_to_grid_coordinate(window->id, window->x, window->y);
        resize_window_with_grid_units(window->id,
            window->height, window->width);
        border_update(window->id);
        element = element->next;
    }

    commit();
}
