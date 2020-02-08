#include <stdlib.h>
#include <string.h>
#include <xcb/randr.h>

#include "geometry.h"
#include "monitor.h"

#include "../vector.h"

#include "../xcb/connection.h"
#include "../xcb/xrandr.h"

vector_t* monitors;

void setup_monitors() {
    monitors = construct_vector();

    monitor_t* monitor;
    if (!xrandr_is_available()) {
        monitor = (monitor_t*)malloc(sizeof(monitor_t));
        monitor->name = (char*)malloc(sizeof(char));
        monitor->geometry = (screen_geometry_t*)malloc(
            sizeof(screen_geometry_t));
        monitor->configuration = NULL;

        monitor->geometry->x = monitor->geometry->y = 0.0;
        monitor->geometry->height = (float)xcb_screen->height_in_pixels;
        monitor->geometry->width = (float)xcb_screen->width_in_pixels;

        monitor->geometries = NULL;

        strcpy(monitor->name, "<xorg>");
        push_to_vector(monitors, monitor);

        return;
    }

    xcb_randr_get_monitors_reply_t* outputs = get_xrandr_outputs();

    xcb_randr_monitor_info_iterator_t iterator;
    iterator = xcb_randr_get_monitors_monitors_iterator(outputs);

    xcb_randr_monitor_info_t* monitor_information;
    char* output_name;

    while (iterator.rem) {
        monitor = (monitor_t*)malloc(sizeof(monitor_t));
        monitor->name = (char*)malloc(sizeof(char));
        monitor->geometry = (screen_geometry_t*)malloc(
            sizeof(screen_geometry_t));
        monitor->configuration = NULL;

        monitor_information = iterator.data;
        output_name = xcb_get_atom_name_name(
            xcb_get_atom_name_reply(xcb_connection,
                xcb_get_atom_name(xcb_connection, monitor_information->name),
            NULL));

        monitor->geometry->x = (float)monitor_information->x;
        monitor->geometry->y = (float)monitor_information->y;

        monitor->geometry->height = (float)monitor_information->height;
        monitor->geometry->width = (float)monitor_information->width;

        monitor->geometries = NULL;

        strcpy(monitor->name, output_name);
        push_to_vector(monitors, monitor);

        xcb_randr_monitor_info_next(&iterator);
    }
}

monitor_t* monitor_from_name(char* name) {
    monitor_t* monitor;
    for (unsigned int index = 0; index < monitors->size; index++) {
        monitor = get_from_vector(monitors, index);

        if (!strcmp(monitor->name, name))
            return monitor;
    }

    return NULL;
}

monitor_t* monitor_with_cursor_residence() {
    xcb_query_pointer_cookie_t pointer_cookie;
    pointer_cookie = xcb_query_pointer(xcb_connection, xcb_screen->root);

    xcb_query_pointer_reply_t* pointer = xcb_query_pointer_reply(xcb_connection,
        pointer_cookie, NULL);

    float x = 0;
    float y = 0;

    monitor_t* monitor;
    for (unsigned int index = 0; index < monitors->size; index++) {
        monitor = get_from_vector(monitors, index);

        x += monitor->geometry->width;
        y += monitor->geometry->height;

        if (pointer->root_x < x && pointer->root_y < y)
            return monitor;
    }

    return NULL;
}
