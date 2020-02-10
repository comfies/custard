#include <stdlib.h>
#include <xcb/xcb.h>

#include "custard.h"
#include "decorations.h"
#include "handlers.h"
#include "window.h"

#include "../xcb/connection.h"
#include "../xcb/window.h"

void (*xcb_events[])(xcb_generic_event_t*) = {
    [XCB_MAP_REQUEST]    = handle_map_request,
    [XCB_DESTROY_NOTIFY] = handle_window_close,
    [XCB_BUTTON_PRESS]   = handle_window_click,
    [XCB_NO_OPERATION]   = NULL
};

void (*signals[])(int) = {
    [SIGINT]    = handle_termination_signal,
    [SIGTERM]   = handle_termination_signal,
    [SIGUNUSED] = NULL
};

/* XCB event handlers */

void handle_map_request(xcb_generic_event_t* generic_event) {
    xcb_map_request_event_t* event;
    event = (xcb_map_request_event_t*)generic_event;

    xcb_window_t window_id = event->window;

    if (window_should_be_managed(window_id)) {
        window_t* window = manage_window(window_id);
        map_window(window_id);
        raise_window(window_id);
        focus_on_window(window);
        decorate(window);
    } else {
        map_window(window_id);
        raise_window(window_id);

        focused_window = window_id;
        focus_window(window_id);
    }

    apply();
}

void handle_window_close(xcb_generic_event_t* generic_event) {
    xcb_destroy_notify_event_t* event;
    event = (xcb_destroy_notify_event_t*)generic_event;

    xcb_window_t window_id = event->window;

    // is the window managed?
    if (window_is_managed(window_id))
        unmanage_window(window_id);

    if (focused_window == window_id)
        focused_window = XCB_WINDOW_NONE;

    apply();
}

void handle_window_click(xcb_generic_event_t* generic_event) {
    xcb_button_press_event_t* event;
    event = (xcb_button_press_event_t*)generic_event;

    xcb_window_t window_id = event->event;

    window_t* window;
    if (windows) {
        for (unsigned int index = 0; index < windows->size; index++) {
            window = get_from_vector(windows, index);

            if (window->parent == window_id) {
                window_id = window->id;
                break;
            }
        }
    } // redirect the parent to the child

    log("Window(%08x) clicked", window_id);

    if (window_is_managed(window_id))
        focus_on_window(get_window_by_id(window_id));
    else {
        xcb_window_t previous_window = focused_window;
        focused_window = window_id;

        if (window_is_managed(previous_window)) {
            xcb_grab_button(xcb_connection, 0, previous_window,
                XCB_EVENT_MASK_BUTTON_PRESS,
                XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
                XCB_NONE, XCB_NONE,
                XCB_BUTTON_INDEX_ANY, XCB_MOD_MASK_ANY);
            decorate(get_window_by_id(previous_window));
        }

        focus_window(window_id);
    }

    apply();
}

// TODO: handle fullscreen toggles

/* Signal handlers */

void handle_termination_signal(int signal) {
    if (signal == SIGINT)
        log("SIGINT received");
    else
        log("SIGTERM received");

    custard_is_running = 0;

    /* The window manager won't die until the next iteration of the loop,
     * so let's just force it to close. */

    finalize();
    exit(EXIT_SUCCESS);
}

// TODO: handle more signals?
