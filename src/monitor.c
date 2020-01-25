#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <xcb/randr.h>

#include "custard.h"
#include "grid.h"
#include "monitor.h"
#include "vector.h"
#include "xcb.h"

unsigned short initialize_monitors() {
    if (!xcb_get_extension_data(xcb_connection, &xcb_randr_id)) {
        monitor_t *monitor = (monitor_t *)malloc(sizeof(monitor_t));

        monitor->x = 0;
        monitor->y = 0;
        monitor->height = screen->height_in_pixels;
        monitor->width = screen->width_in_pixels;

        monitor->grid = (grid_t *)malloc(sizeof(grid_t));

        monitor->name = (char *)malloc(sizeof(char));
        strcpy(monitor->name, "X");

        debug_output("Monitor initialization failed; %s",
                "xrandr extension unavailable, initialized entire screen");

        push_to_vector(monitors, monitor);

        return 0;
    }

    xcb_randr_monitor_info_t *monitor_information;

    xcb_randr_get_monitors_reply_t *outputs = xcb_randr_get_monitors_reply(
            xcb_connection, xcb_randr_get_monitors(
                xcb_connection, screen->root, XCB_NONE), NULL);
    xcb_randr_monitor_info_iterator_t monitor_iterator;

    monitor_iterator = xcb_randr_get_monitors_monitors_iterator(outputs);

    monitor_t *monitor;
    char *monitor_name;
    while (monitor_iterator.rem) {
        monitor_information = monitor_iterator.data;

        monitor = (monitor_t *)malloc(sizeof(monitor_t));

        monitor_name = xcb_get_atom_name_name(xcb_get_atom_name_reply(
                    xcb_connection, xcb_get_atom_name(
                        xcb_connection, monitor_information->name), NULL));

        monitor->x = monitor_information->x;
        monitor->y = monitor_information->y;
        monitor->height = monitor_information->height;
        monitor->width = monitor_information->width;

        monitor->grid = (grid_t *)malloc(sizeof(grid_t));
        
        monitor->name = (char *)malloc(sizeof(char));
        strcpy(monitor->name, monitor_name);

        debug_output("Monitor %s %dx%d +%d+%d",
                monitor->name, monitor->width, monitor->height,
                monitor->x, monitor->y);

        push_to_vector(monitors, monitor);
        debug_output("%s %s", monitor->name, monitor_name);
        xcb_randr_monitor_info_next(&monitor_iterator);
    }

    return 1;
}

monitor_t *get_focused_monitor() {
    // The focused monitor is the one with the cursor on it.
    // Not much more complex than that.
    
    xcb_query_pointer_reply_t *pointer = xcb_query_pointer_reply(xcb_connection,
            xcb_query_pointer(xcb_connection, screen->root), NULL);

    unsigned int cursor_x = (unsigned int)pointer->root_x;
    unsigned int cursor_y = (unsigned int)pointer->root_y;

    unsigned int summation_x = 0;
    unsigned int summation_y = 0;

    monitor_t *monitor = NULL;
    for (unsigned int index = 0; index < monitors->size; index++) {
        monitor = get_from_vector(monitors, index);

        if (cursor_x < (summation_x + monitor->width) &&
                cursor_y < (summation_y + monitor->height))
            return monitor;

        summation_x += monitor->width;
        summation_y += monitor->height;
    }

    return NULL;
}
