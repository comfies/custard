#include <stdlib.h>
#include <string.h>

#include "ipc.h"
#include "parsing.h"

#include "../wm/custard.h"
#include "../wm/decorations.h"
#include "../wm/geometry.h"
#include "../wm/monitor.h"
#include "../wm/rules.h"
#include "../wm/window.h"
#include "../wm/workspace.h"
#include "../xcb/connection.h"
#include "../xcb/window.h"

void ipc_process_input(char *feed) {
    unsigned short update = 0;
    vector_t *input = construct_vector();

    char *token;
    while ((token = strsep(&feed, "\31")))
        push_to_vector(input, token);

    char *qualifier = vector_iterator(input);

    if (!strcmp(qualifier, "halt")) {
        custard_is_running = 0;
    } else if (!strcmp(qualifier, "configure"))
        ipc_command_configure(input, &update);
    else if (!strcmp(qualifier, "geometry"))
        ipc_command_geometry(input, &update);
    else if (!strcmp(qualifier, "match"))
        ipc_command_match(input, &update);
    else if (!strcmp(qualifier, "window"))
        ipc_command_window(input, &update);
    else if (!strcmp(qualifier, "workspace"))
        ipc_command_workspace(input, &update);

    if (update) {
        apply();
        update = 0;
    }

    deconstruct_vector(input);
}

void ipc_helper_typecast_and_assign(kv_value_t *kv_value, char *variable,
    char *input) {

    // Assign all color values

    if (!strcmp(variable, "border.color.focused") ||
        !strcmp(variable, "border.color.unfocused") ||
        !strcmp(variable, "border.color.background")) {
        kv_value->color = string_to_color(input);
        return;
    }

    // Assign all boolean values

    if (!strcmp(variable, "border.colors.flipped")) {
        kv_value->boolean = string_to_boolean(input);
        return;
    }

    // Assign all string values

    if (!strcmp(variable, "geometry") ||
        !strcmp(variable, "monitor")) {
        kv_value->string = (char*)malloc(sizeof(char));
        strcpy(kv_value->string, input);
        return;
    }

    // If all else fails, it's an integer

    kv_value->number = string_to_integer(input);
}

void ipc_command_configure(vector_t *input, unsigned short *screen_update) {
    /*
     * Usage:
     *  custard - configure ([configurable] [value])...
     */

    char *variable;
    char *value_string;
    kv_value_t *value = NULL;

    while (input->remaining) {
        variable = vector_iterator(input);
        value_string = vector_iterator(input);

        if (!(value = get_value_from_key(configuration, variable)))
            continue;

        ipc_helper_typecast_and_assign(value, variable, value_string);
    }

    if (!windows)
        return;

    window_t *window;
    while ((window = vector_iterator(windows))) {
        set_window_geometry(window, window->geometry);
        decorate(window);
    }

    *screen_update = 1;
}

void ipc_command_geometry(vector_t *input, unsigned short *screen_update) {
    suppress_unused(screen_update);

    /*
     * Usage:
     *  custard - geometry [monitor or '*'] [label] [width]x[height] [x],[y]
     */

    char *monitor_name;
    char *label;
    char *size;
    char *position;

    char *token;
    unsigned int height;
    unsigned int width;
    unsigned int x;
    unsigned int y;

    monitor_t *monitor;
    grid_geometry_t *geometry = NULL;
    labeled_grid_geometry_t *labeled_geometry = NULL;

    // Missing geometry data
    if ((input->size - 1) % 4) return;

    while (input->remaining) {
        labeled_geometry = NULL;
        geometry = NULL;

        monitor_name = vector_iterator(input);
        label = vector_iterator(input);
        size = vector_iterator(input);
        position = vector_iterator(input);

        token = strsep(&size, "x");

        width = string_to_integer(token);
        height = string_to_integer(size);

        token = strsep(&position, ",");

        x = string_to_integer(token);
        y = string_to_integer(position);

        /* bullshit here */

        while ((monitor = vector_iterator(monitors))) {
            if (!strcmp(monitor_name, "*") ||
                !strcmp(monitor_name, monitor->name)) {

                if (monitor->geometries) {
                    geometry = get_geometry_from_monitor(monitor, label);
                    if (geometry) {
                        geometry->x = x;
                        geometry->y = y;
                        geometry->height = height;
                        geometry->width = width;
                        free(labeled_geometry);

                        continue;
                    }
                } else
                    monitor->geometries = construct_vector();

                labeled_geometry = create_labeled_geometry(label,
                    x, y, height, width);
                push_to_vector(monitor->geometries, labeled_geometry);
            }
        }
    }

}

void ipc_command_match(vector_t *input, unsigned short *screen_update) {
    suppress_unused(screen_update);

    if (input->size < 3) return;

    char *subject = vector_iterator(input);

    if (!strcmp(subject, "monitor"))
        ipc_sub_command_match_monitor(input);
    else
        ipc_sub_command_match_window(input);
}

void ipc_command_window(vector_t *input, unsigned short *screen_update) {
    // Missing input
    if (input->size < 2) return;
    char *variable = vector_iterator(input);

    /*
     * Usage:
     * custard - window close
     *  custard - window geometry [label]
     */

    window_t *window = NULL;
    if (focused_window == XCB_WINDOW_NONE)
        return;
    if (window_is_managed(focused_window))
        window = get_window_by_id(focused_window);

    if (!strcmp(variable, "close")) {
        close_window(focused_window);
    } else if (!strcmp(variable, "raise")) {

        if (window)
            raise_window(window->parent);
        else
            raise_window(focused_window);

    } else if (!strcmp(variable, "expand")) {

        if (!window)
            return;

        char *cardinal = vector_iterator(input);

        if (!strcmp(cardinal, "north")) {
            window->geometry->y--;
            window->geometry->height++;
        }

        if (!strcmp(cardinal, "south"))
            window->geometry->height++;

        if (!strcmp(cardinal, "east"))
            window->geometry->width++;

        if (!strcmp(cardinal, "west")) {
            window->geometry->x--;
            window->geometry->width++;
        }

        set_window_geometry(window, window->geometry);
        decorate(window);

    } else if (!strcmp(variable, "contract")) {

        if (!window)
            return;

        char *cardinal = vector_iterator(input);

        if (!strcmp(cardinal, "north")) {
            window->geometry->y++;
            window->geometry->height--;
        }

        if (!strcmp(cardinal, "south"))
            window->geometry->height--;

        if (!strcmp(cardinal, "east"))
            window->geometry->width--;

        if (!strcmp(cardinal, "west")) {
            window->geometry->x++;
            window->geometry->width--;
        }

        set_window_geometry(window, window->geometry);
        decorate(window);

    } else if (!strcmp(variable, "lower")) {

        if (window)
            lower_window(window->parent);
        else
            lower_window(focused_window);

    } else if (!strcmp(variable, "workspace")) {

        unsigned int workspace = string_to_integer(vector_iterator(input));

        if (window) {
            if (window->workspace == workspace)
                return;

            window->workspace = workspace;

            monitor_t *monitor = monitor_with_cursor_residence();
            if (monitor->workspace != window->workspace)
                unmap_window(window->parent);
        }

    } else if (!strcmp(variable, "geometry")) {
        char *label = vector_iterator(input);

        grid_geometry_t *geometry;
        monitor_t *monitor = monitor_with_cursor_residence();

        log_debug("Looking for geometry(%s) in monitor(%s)",
            label, monitor->name);

        geometry = get_geometry_from_monitor(monitor, label);

        if (!geometry)
            return;

        window_t *window = get_window_by_id(focused_window);
        if (window) {
            set_window_geometry(window, geometry);
            decorate(window);
        }

    }

    *screen_update = 1;
}

void ipc_command_workspace(vector_t *input, unsigned short *screen_update) {
    monitor_t *monitor = monitor_with_cursor_residence();

    unsigned int workspace = string_to_integer(vector_iterator(input));
    if (workspace) {
        show_workspace_on_monitor(monitor, workspace);
    }

    *screen_update = 1;
}

/* Sub-commands */

void ipc_sub_command_match_monitor(vector_t *input) {
    char *monitor_name = vector_iterator(input);
    monitor_t *monitor = monitor_from_name(monitor_name);

    if (!monitor)
        return;

    if (!monitor->configuration)
        monitor->configuration = construct_vector();

    char *configurables[7] = {
        "grid.rows,"
        "grid.columns",
        "grid.margins",
        "grid.margin.top",
        "grid.margin.bottom",
        "grid.margin.left",
        "grid.margin.right"
    };
    unsigned int index = 0;
    kv_pair_t *setting;

    char *variable;
    char *value_string;

    while (input->remaining) {
        variable = vector_iterator(input);
        value_string = vector_iterator(input);

        log_debug("%s = %s", variable, value_string);

        for (index = 0; index < 7;) {
            if (!strcmp(configurables[index++], variable)) {
                setting = create_or_get_kv_pair(monitor->configuration,
                    variable);
                setting->value->number = string_to_integer(value_string);
                break;
            }
        }
    }

}

void ipc_sub_command_match_window(vector_t *input) {
    /*
     * Usage:
     *  custard - match window.name [expression] ([configurable] [value])...
     * custard - match window.class [expression] ([configurable] [value])...
     */

    input->remaining++;
    char *subject = vector_iterator(input);
    char *expression = vector_iterator(input);

    window_attribute_t attribute;

    if (!strcmp(subject, "window.name"))
        attribute = name;
    else if (!strcmp(subject, "window.class"))
        attribute = class;
    else return;

    rule_t *rule = create_or_get_rule(attribute, expression);
    if (!rule->rules)
        rule->rules = construct_vector();

    char *configurables[10] = {
        "borders",
        "border.size.outer",
        "border.size.inner",
        "border.color.focused",
        "border.color.unfocused",
        "border.color.background",
        "border.colors.flipped",
        "geometry",
        "monitor",
        "workspace"
    };
    unsigned int index = 0;
    kv_pair_t *setting;

    char *variable;
    char *value_string;

    while (input->remaining > 0) {
        variable = vector_iterator(input);
        value_string = vector_iterator(input);

        for (index = 0; index < 10;) {
            if (!strcmp(configurables[index++], variable)) {
                setting = create_or_get_kv_pair(rule->rules, variable);
                ipc_helper_typecast_and_assign(setting->value,
                    variable, value_string);
                break;
            }
        }
    }

    add_rule(rule);
}
