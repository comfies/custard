#include "ipc.h"

#include "config.h"
#include "grid.h"
#include "utilities.h"
#include "window.h"
#include "workspace.h"
#include "xcb.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ipc_command ipc_commands[][NUMCOMMANDS] = {
    {{"custard",     "stop",             ipc_custard_halt}},
    {{"custard",     "configure",        ipc_custard_configure}},
    {{"custard",     "new geometry",     ipc_custard_new_geometry}},
    {{"custard",     "new rule",         ipc_custard_new_rule}},

    {{"window",      "close",            ipc_window_close}},
    {{"window",      "raise",            ipc_window_raise}},
    {{"window",      "lower",            ipc_window_lower}},
    {{"window",      "change geometry",  ipc_window_change_geometry}},

    {{"workspace",   "focus",            ipc_workspace_focus}}
};

void
process_command(char *input)
{
    debug_output("beginning of received input >>\n %s <<end of received input", input);

    char *target;
    char *action;
    char *arguments[8];

    char *token;
    unsigned int index = 0;

    token = strtok(input, ";");

    while (token) {
        if (index == 0) {
            target = token;
        } else if (index == 1) {
            action = token;
        } else {
            arguments[index - 2] = token;
        }

        token = NULL;
        if (++index < 10) {
            token = strtok(NULL, ";");
        }
    }

    if (!target || !action) {
        return;
    }

    debug_output("Input split");

    short unsigned flush;
    struct ipc_command *command;
    for (unsigned int index = 0; index < NUMCOMMANDS; index++) {
        command = ipc_commands[index];

        if (strcmp(target, command->target))
            continue;

        if (strcmp(action, command->action))
            continue;

        /* Found the match, now what? */
        flush = command->routine(arguments);

        if (flush)
            commit();

        return;
    }

    return;
}

short unsigned
ipc_custard_halt(char **arguments) 
{
    wm_running = 0;
    return 0;
}

short unsigned
ipc_custard_configure(char **arguments)
{
    if (strcmp("grid_rows", arguments[0]) == 0) {
        grid_rows = parse_unsigned_integer(arguments[1]);
    } else if (strcmp("grid_columns", arguments[0]) == 0) {
        grid_columns = parse_unsigned_integer(arguments[1]);
    } else if (strcmp("grid_gap", arguments[0]) == 0) {
        grid_gap = parse_unsigned_integer(arguments[1]);
    } else if (strcmp("grid_margin_top", arguments[0]) == 0) {
        grid_margin_top = parse_unsigned_integer(arguments[1]);
    } else if (strcmp("grid_margin_bottom", arguments[0]) == 0) {
        grid_margin_bottom = parse_unsigned_integer(arguments[1]);
    } else if (strcmp("grid_margin_left", arguments[0]) == 0) {
        grid_margin_left = parse_unsigned_integer(arguments[1]);
    } else if (strcmp("grid_margin_right", arguments[0]) == 0) {
        grid_margin_right = parse_unsigned_integer(arguments[1]);
    } else if (strcmp("border_type", arguments[0]) == 0) {
        border_type = parse_unsigned_integer(arguments[1]);

        if (border_type > 3) {
            border_type = 3;
        }
    } else if (strcmp("border_inner_size", arguments[0]) == 0) {
        border_inner_size = parse_unsigned_integer(arguments[1]);

        if (border_type == 0) {
            border_total_size = 0;
        } else {
            border_total_size = border_inner_size +
                ((border_type - 1) * border_outer_size);
        }
    } else if (strcmp("border_outer_size", arguments[0]) == 0) {
        border_outer_size = parse_unsigned_integer(arguments[1]);

        if (border_type == 0) {
            border_total_size = 0;
        } else {
            border_total_size = border_inner_size +
                ((border_type - 1) * border_outer_size);
        }

    } else if (strcmp("border_focused_color", arguments[0]) == 0) {
        border_focused_color = parse_rgba_color(arguments[1]);
    } else if (
        strcmp("border_unfocused_color", arguments[0]) == 0) {
        border_unfocused_color = parse_rgba_color(arguments[1]);
    } else if (
        strcmp("border_background_color", arguments[0]) == 0) {
        border_background_color = parse_rgba_color(arguments[1]);
    }

                /* Missing: border_invert_colors, workspaces */

    grid_apply_configuration();

    return 1;
}

short unsigned
ipc_custard_new_geometry(char **arguments)
{
    new_geometry(arguments[0],
        parse_unsigned_integer(arguments[1]),
        parse_unsigned_integer(arguments[2]),
        parse_unsigned_integer(arguments[3]),
        parse_unsigned_integer(arguments[4]));

    return 0;
}

short unsigned
ipc_custard_new_rule(char **arguments)
{
    new_geometry_rule(
        (window_attribute_t)parse_unsigned_integer(arguments[0]),
        arguments[1],
        arguments[2]);

    return 0;
}

short unsigned
ipc_window_close(char **arguments)
{
    if (!focused_window)
        return 0;

    xcb_window_t window_id = focused_window->id;

    close_window(window_id);

    return 1;
}

short unsigned
ipc_window_raise(char **arguments)
{
    if (!focused_window)
        return 0;

    xcb_window_t window_id = focused_window->id;

    raise_window(window_id);

    return 1;
}

short unsigned
ipc_window_lower(char **arguments)
{
    if (!focused_window)
        return 0;

    xcb_window_t window_id = focused_window->id;

    lower_window(window_id);

    return 1;
}

short unsigned
ipc_window_change_geometry(char **arguments)
{
    if (!focused_window)
        return 0;

    xcb_window_t window_id = focused_window->id;

    struct LinkedListElement *element = geometry_list_head;

    if (!element)
        return 0;

    while (element) {
        debug_output("Testing geometry %s to %s.",
            ((Geometry *)element->data)->name, arguments[0]);
        if (strcmp(((Geometry *)element->data)->name,
            arguments[0]) == 0) {

            change_window_geometry_grid_coordinate(window_id,
                ((Geometry *)element->data)->x,
                ((Geometry *)element->data)->y,
                ((Geometry *)element->data)->height,
                ((Geometry *)element->data)->width);

            border_update(window_id);

            return 1;
        }

        element = element->next;
    }

    return 0;
}

short unsigned
ipc_workspace_focus(char **arguments)
{
    unsigned int workspace = parse_unsigned_integer(arguments[0]);

    focus_on_workspace(workspace);

    return 1;
}
