#include <stdlib.h>
#include <string.h>

#include "ipc.h"
#include "parsing.h"

#include "../wm/config.h"
#include "../wm/custard.h"
#include "../wm/geometry.h"
#include "../wm/monitor.h"
#include "../wm/rules.h"
#include "../wm/window.h"
#include "../xcb/connection.h"

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

    if (update) {
        apply();
        update = 0;
    }

    deconstruct_vector(input);
}

void ipc_command_configure(vector_t* input, unsigned short* screen_update) {
    suppress_unused(screen_update);

    char* variable;
    char* value_string;

    kv_value_t* value = NULL;
    for (unsigned int index = 1; index < input->size; index += 2) {
        variable = get_from_vector(input, index);
        value_string = get_from_vector(input, index + 1);

        value = get_value_from_key(configuration, variable);

        if (!value)
            continue;

        if (!strcmp(variable, "border.colors.focused"))
            value->boolean = string_to_boolean(value_string);
        if (!strcmp(variable, "border.color.background") ||
            !strcmp(variable, "border.color.focused") ||
            !strcmp(variable, "border.color.unfocused"))
            value->number = string_to_color(value_string);
        else
            value->number = string_to_integer(value_string);
    }
}

void ipc_command_geometry(vector_t* input, unsigned short* screen_update) {
    suppress_unused(screen_update);

    char* monitor_name = get_from_vector(input, 1);

    char* label = get_from_vector(input, 2);
    char* size = get_from_vector(input, 3);
    char* position = get_from_vector(input, 4);

    char* token;

    token = strsep(&size, "x");

    unsigned int width = string_to_integer(token);
    unsigned int height = string_to_integer(size);

    token = strsep(&position, ",");

    unsigned int x = string_to_integer(token);
    unsigned int y = string_to_integer(position);

    monitor_t* monitor;
    grid_geometry_t* geometry = NULL;
    labeled_grid_geometry_t* labeled_geometry = NULL;

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

void ipc_command_match(vector_t* input, unsigned short* screen_update) {
    suppress_unused(screen_update);

    char* subject = get_from_vector(input, 1);
    char* expression = get_from_vector(input, 2);

    if (!strcmp(subject, "monitor")) {
        // do shit
    } else {

        /*
         * Usage:
         *  custard - match window.name [expression] [configurable value]...
         *  custard - match window.class [expression] [configurable value]...
         */

        window_attribute_t attribute;
        if (!strcmp(subject, "window.name")) {
            attribute = name;
        } else if (!strcmp(subject, "window.class")) {
            attribute = class;
        } else return;

        char* configurables[9] = {
            "borders",
            "border.size.inner",
            "border.size.outer",
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

        char* variable;
        char* configurable = NULL;
        char* value_string;
        kv_pair_t* setting;
        for (unsigned int index = 3; index < input->size; index += 2) {
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
            } else if (!strcmp(variable, "border.colors.flipped")) {
                setting->value->boolean = string_to_boolean(value_string);
            } else {
                setting->value->number = string_to_integer(value_string);
            }

            configurable = NULL;
        }

        add_rule(rule);

    }
}
