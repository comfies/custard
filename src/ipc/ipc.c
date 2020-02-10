#include <stdlib.h>
#include <string.h>

#include "ipc.h"
#include "parsing.h"

#include "../wm/config.h"
#include "../wm/custard.h"
#include "../wm/decorations.h"
#include "../wm/geometry.h"
#include "../wm/monitor.h"
#include "../wm/rules.h"
#include "../wm/window.h"
#include "../xcb/connection.h"
#include "../xcb/window.h"

void ipc_process_input(char* feed) {
    unsigned short update = 0;
    vector_t* input = construct_vector();

    char* token;
    while ((token = strsep(&feed, "\31")))
        push_to_vector(input, token);

    char* qualifier = get_from_vector(input, 0);

    if (!strcmp(qualifier, "configure"))
        ipc_command_configure(input, &update);
    else if (!strcmp(qualifier, "geometry"))
        ipc_command_geometry(input, &update);
    else if (!strcmp(qualifier, "match"))
        ipc_command_match(input, &update);
    else if (!strcmp(qualifier, "window"))
        ipc_command_window(input, &update);

    if (update) {
        apply();
        update = 0;
    }

    deconstruct_vector(input);
}

void ipc_command_configure(vector_t* input, unsigned short* screen_update) {
    /*
     * Usage:
     *  custard - configure ([configurable] [value])...
     */

    char* variable;
    char* value_string;
    unsigned int index = 1;

    // There's a missing setting name-value pair
    if (((input->size - 1) % 2)) return;

    kv_value_t* value = NULL;
    for (; index < input->size; index += 2) {
        variable = get_from_vector(input, index);
        value_string = get_from_vector(input, index + 1);

        value = get_value_from_key(configuration, variable);

        if (!value)
            continue;

        if (!strcmp(variable, "border.colors.flipped"))
            value->boolean = string_to_boolean(value_string);
        else if (!strcmp(variable, "border.color.background") ||
            !strcmp(variable, "border.color.focused") ||
            !strcmp(variable, "border.color.unfocused"))
            value->color = string_to_color(value_string);
        else
            value->number = string_to_integer(value_string);
    }
    index = 0;

    if (!windows)
        return;

    window_t* window;
    for (; index < windows->size; index++) {
        window = get_from_vector(windows, index);

        set_window_geometry(window, window->geometry);
        decorate(window);
    }

    *screen_update = 1;
}

void ipc_command_geometry(vector_t* input, unsigned short* screen_update) {
    suppress_unused(screen_update);

    /*
     * Usage:
     *  custard - geometry [monitor or '*'] [label] [width]x[height] [x],[y]
     */

    char* monitor_name;
    char* label;
    char* size;
    char* position;

    char* token;
    unsigned int height;
    unsigned int width;
    unsigned int x;
    unsigned int y;

    monitor_t* monitor;
    grid_geometry_t* geometry = NULL;
    labeled_grid_geometry_t* labeled_geometry = NULL;

    // Missing geometry data
    if ((input->size - 1) % 4) return;

    for (unsigned int index = 1; index < input->size; index += 4) {
        labeled_geometry = NULL;
        geometry = NULL;

        monitor_name = get_from_vector(input, index);
        label = get_from_vector(input, index + 1);
        size = get_from_vector(input, index + 2);
        position = get_from_vector(input, index + 3);

        token = strsep(&size, "x");

        width = string_to_integer(token);
        height = string_to_integer(size);

        token = strsep(&position, ",");

        x = string_to_integer(token);
        y = string_to_integer(position);

        if (strcmp(monitor_name, "*")) {
            monitor = monitor_from_name(monitor_name);

            if (!monitor)
                return;

            geometry = get_geometry_from_monitor(monitor, label);
            if (geometry) {
                geometry->x = x;
                geometry->y = y;
                geometry->height = height;
                geometry->width = width;
            } else {
                if (!monitor->geometries)
                    monitor->geometries = construct_vector();

                labeled_geometry = create_labeled_geometry(label,
                    x, y, height, width);
                push_to_vector(monitor->geometries, labeled_geometry);
            }

        } else {
            for (unsigned int index = 0; index < monitors->size; index++) {
                monitor = get_from_vector(monitors, index);

                geometry = get_geometry_from_monitor(monitor, label);
                if (geometry) {
                    geometry->x = x;
                    geometry->y = y;
                    geometry->height = height;
                    geometry->width = width;
                    break; // everything using this geometry is good now,
                    // should iterate once since this would be a global geometry
                } else {
                    if (!monitor->geometries)
                        monitor->geometries = construct_vector();

                    if (!labeled_geometry)
                        labeled_geometry = create_labeled_geometry(label,
                            x, y, height, width);
                    push_to_vector(monitor->geometries, labeled_geometry);
                }
            }
        }

        log("Geometry (%s[%s]: %d,%d %dx%d)", label, monitor_name,
            x, y, width, height);
    }

    return;
}

void ipc_command_match(vector_t* input, unsigned short* screen_update) {
    suppress_unused(screen_update);

    // There is definitely a better way of programming this.
    // For now, sleep deprived code works.
    // In fact, the whole IPC is rather shitty. But it works. For now.

    // missing input
    if (input->size < 3) return;

    char* subject = get_from_vector(input, 1);
    char* expression = get_from_vector(input, 2);

    char* variable;
    char* value_string;
    kv_pair_t* setting;
    char* configurable = NULL;
    unsigned int index = 0;

    if (!strcmp(subject, "monitor")) {
        // do shit
        char* configurables[5] = {
            "grid.rows",
            "grid.columns",
            "grid.margins",
            "grid.margin.top",
            "grid.margin.bottom"
        };

        monitor_t* monitor;
        for (; index < monitors->size; index++) {
            monitor = get_from_vector(monitors, index);

            if (!strcmp(expression, monitor->name)) {
                if (!monitor->configuration)
                    monitor->configuration = construct_vector();

                for (index = 3; index < input->size; index += 2) {
                    variable = get_from_vector(input, index);
                    value_string = get_from_vector(input, index + 1);

                    for (unsigned int sub_index = 0; sub_index < 5; sub_index++) {
                        if (!strcmp(configurables[sub_index], variable)) {
                            configurable = variable;
                            break;
                        }
                    }

                    if (!configurable)
                        continue;

                    setting = create_or_get_kv_pair(monitor->configuration,
                        variable);
                    setting->value->number = string_to_integer(value_string);

                    configurable = NULL;
                }

                return;
            }
        }
    } else {

        /*
         * Usage:
         *  custard - match window.name [expression] ([configurable] [value])...
         * custard - match window.class [expression] ([configurable] [value])...
         */

        // Missing setting name-value pair
        if ((input->size - 3) % 2) return;

        window_attribute_t attribute;
        if (!strcmp(subject, "window.name")) {
            attribute = name;
        } else if (!strcmp(subject, "window.class")) {
            attribute = class;
        } else return;

        char* configurables[9] = {
            "borders",
            "border.size.outer",
            "border.size.inner",
            "border.color.focused",
            "border.color.unfocused",
            "border.color.background",
            "border.colors.flipped",
            "geometry",
            "monitor"
            // "workspace" eventually
        };

        rule_t* rule = create_or_get_rule(attribute, expression);
        rule->rules = construct_vector();

        index = 3;
        for (; index < input->size; index += 2) {
            variable = get_from_vector(input, index);
            value_string = get_from_vector(input, index + 1);

            for (unsigned int sub_index = 0; sub_index < 9; sub_index++) {
                if (!strcmp(configurables[sub_index], variable)) {
                    configurable = variable;
                    break;
                }
            }

            if (!configurable)
                continue;

            setting = create_or_get_kv_pair(rule->rules, variable);
            if (!strcmp(variable, "geometry") ||
                !strcmp(variable, "monitor")) {
                setting->value->string = (char*)malloc(sizeof(char));
                strcpy(setting->value->string, value_string);
            } else if (!strcmp(variable, "border.colors.flipped"))
                setting->value->boolean = string_to_boolean(value_string);
            else if (!strcmp(variable, "border.color.background") ||
                !strcmp(variable, "border.color.focused") ||
                !strcmp(variable, "border.color.unfocused"))
                setting->value->color = string_to_color(value_string);
            else
                setting->value->number = string_to_integer(value_string);

            configurable = NULL;
        }

        add_rule(rule);

    }
}

void ipc_command_window(vector_t* input, unsigned short* screen_update) {
    // Missing input
    if (input->size < 2) return;
    char* variable = get_from_vector(input, 1);

    /*
     * Usage:
     * custard - window close
     *  custard - window geometry [label]
     */

    if (!strcmp(variable, "close")) {
        close_window(focused_window);
        *screen_update = 1;
    } else if (!strcmp(variable, "geometry")) {
        char* label = get_from_vector(input, 2);
        grid_geometry_t* geometry;

        geometry = get_geometry_from_monitor(
            monitor_with_cursor_residence(), label);

        if (!geometry)
            return;

        window_t* window = get_window_by_id(focused_window);
        if (window) {
            set_window_geometry(window, geometry);
            decorate(window);
            *screen_update = 1;
        }

    }

}
