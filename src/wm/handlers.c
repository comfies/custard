#include <stdlib.h>
#include <xcb/xcb.h>

#include "custard.h"
#include "handlers.h"
#include "window.h"

#include "../xcb/connection.h"
#include "../xcb/window.h"

void (*xcb_events[])(xcb_generic_event_t*) = {
    [XCB_MAP_REQUEST]  = handle_map_request,
    [XCB_DESTROY_NOTIFY] = handle_window_close,
    [XCB_NO_OPERATION] = NULL
};

void (*signals[])(int) = {
    [SIGINT]    = handle_sigint,
    [SIGUNUSED] = NULL
};

/* XCB event handlers */

void handle_map_request(xcb_generic_event_t* generic_event) {
    xcb_map_request_event_t* event;
    event = (xcb_map_request_event_t*)generic_event;

    xcb_window_t window_id = event->window;

    map_window(window_id);
    raise_window(window_id);

    if (window_should_be_managed(window_id))
        manage_window(window_id);

    apply();
}

void handle_window_close(xcb_generic_event_t* generic_event) {
    xcb_destroy_notify_event_t* event;
    event = (xcb_destroy_notify_event_t*)generic_event;

    xcb_window_t window_id = event->window;

    // is the window managed?
    unmanage_window(window_id);

    apply();
}

// TODO: handle clicks for window focus
// TODO: handle fullscreen toggles

/* Signal handlers */

void handle_sigint(int signal) {
    suppress_unused(signal);

    log("SIGINT received");
    custard_is_running = 0;

    /* The window manager won't die until the next iteration of the loop,
     * so let's just force it to close. */

    finalize();
    exit(EXIT_SUCCESS);
}

// TODO: handle more signals?
