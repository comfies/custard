#include <stdlib.h>
#include <xcb/randr.h>

#include "custard.h"
#include "monitor.h"
#include "xcb.h"

unsigned short initialize_monitors() {
    if (!xcb_get_extension_data(xcb_connection, &xcb_randr_id))
        return 0;

    xcb_randr_get_screen_resources_reply_t *screen_resources;

    screen_resources = xcb_randr_get_screen_resources_reply(xcb_connection,
            xcb_randr_get_screen_resources(xcb_connection, screen->root), NULL);

    xcb_randr_output_t *monitors = xcb_randr_get_screen_resources_outputs(
            screen_resources);

    unsigned int number_of_monitors;
    number_of_monitors = xcb_randr_get_screen_resources_outputs_length(
            screen_resources);

    xcb_randr_get_output_info_reply_t *output_data;
    xcb_randr_get_crtc_info_reply_t *monitor_data;

    for (unsigned int index = 0; index < number_of_monitors; index++) {
        output_data = xcb_randr_get_output_info_reply(xcb_connection,
                xcb_randr_get_output_info(xcb_connection, monitors[index],
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

        free(monitor_data);
        free(output_data);
    }

    return 1;
}

