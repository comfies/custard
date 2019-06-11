#include <stdlib.h>
#include <xcb/randr.h>

#include "custard.h"
#include "grid.h"
#include "monitor.h"
#include "vector.h"
#include "xcb.h"

unsigned short initialize_monitors() {
    if (!xcb_get_extension_data(xcb_connection, &xcb_randr_id))
        return 0;

    xcb_randr_get_screen_resources_reply_t *screen_resources;

    screen_resources = xcb_randr_get_screen_resources_reply(xcb_connection,
            xcb_randr_get_screen_resources(xcb_connection, screen->root), NULL);

    xcb_randr_output_t *outputs = xcb_randr_get_screen_resources_outputs(
            screen_resources);

    unsigned int number_of_monitors;
    number_of_monitors = xcb_randr_get_screen_resources_outputs_length(
            screen_resources);

    xcb_randr_get_output_info_reply_t *output_data;
    xcb_randr_get_crtc_info_reply_t *monitor_data;

    monitor_t *monitor = NULL;
    for (unsigned int index = 0; index < number_of_monitors; index++) {
        output_data = xcb_randr_get_output_info_reply(xcb_connection,
                xcb_randr_get_output_info(xcb_connection, outputs[index],
                    XCB_CURRENT_TIME), NULL);

        if (!output_data || !output_data->crtc) {
            free(output_data);
            continue;
        }

        monitor_data = xcb_randr_get_crtc_info_reply(xcb_connection,
                xcb_randr_get_crtc_info(xcb_connection, output_data->crtc,
                    XCB_CURRENT_TIME), NULL);

        debug_output("Monitor: %d,%d %dx%d",
                monitor_data->x, monitor_data->y,
                monitor_data->width, monitor_data->height);

        monitor = (monitor_t *)malloc(sizeof(monitor_t));
        monitor->x = monitor_data->x;
        monitor->y = monitor_data->y;
        monitor->height = monitor_data->height;
        monitor->width = monitor_data->width;

        monitor->grid = (grid_t *)malloc(sizeof(grid_t));
        // each monitor has its own grid
        apply_configuration_to_monitor_grid(monitor);

        push_to_vector(monitors, monitor);

        free(monitor_data);
        free(output_data);
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
