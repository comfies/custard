#include <stdlib.h>
#include <xcb/xcb.h>

#include "custard.h"
#include "decorations.h"
#include "handlers.h"
#include "window.h"

#include "../xcb/connection.h"
#include "../xcb/ewmh.h"
#include "../xcb/window.h"

void (*xcb_events[])(xcb_generic_event_t*) = {
    [XCB_MAP_REQUEST]    = handle_map_request,
    [XCB_DESTROY_NOTIFY] = handle_window_close,
    [XCB_CLIENT_MESSAGE] = handle_window_message,
    [XCB_BUTTON_PRESS]   = handle_window_click,
    [XCB_NO_OPERATION]   = NULL
};

void (*signals[])(int) = {
    [SIGINT]    = handle_termination_signal,
    [SIGTERM]   = handle_termination_signal,
    [SIGUNUSED] = NULL
};

/* XCB event handlers */

void handle_map_request(xcb_generic_event_t *generic_event) {
    xcb_map_request_event_t *event;
    event = (xcb_map_request_event_t*)generic_event;

    xcb_window_t window_id = event->window;

    xcb_window_t previously_focused_window = focused_window;
    focused_window = window_id;

    map_window(window_id);

    window_t *window = NULL;
    if (window_should_be_managed(window_id))
        window = manage_window(window_id);

    if (window) {
        raise_window(window->parent);
        decorate(window);
    } else {
        raise_window(window_id);
    }
    focus_window(window_id);

    if (window_is_managed(previously_focused_window)) {
        window = get_window_by_id(previously_focused_window);
        xcb_grab_button(xcb_connection, 0, previously_focused_window,
            XCB_EVENT_MASK_BUTTON_PRESS,
            XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
            XCB_NONE, XCB_NONE,  XCB_BUTTON_INDEX_ANY, XCB_MOD_MASK_ANY);
        decorate(window);
    }

    apply();
}

void handle_window_close(xcb_generic_event_t *generic_event) {
    xcb_destroy_notify_event_t *event;
    event = (xcb_destroy_notify_event_t*)generic_event;

    xcb_window_t window_id = event->window;

    // is the window managed?
    if (window_is_managed(window_id))
        unmanage_window(window_id);

    if (focused_window == window_id)
        focused_window = XCB_WINDOW_NONE;

    apply();
}

void handle_window_message(xcb_generic_event_t *generic_event) {
    xcb_client_message_event_t *event;
    event = (xcb_client_message_event_t*)generic_event;

    xcb_window_t window_id = event->window;

    if (event->type == ewmh_connection->_NET_CLOSE_WINDOW) {
        if (window_is_managed(window_id))
            unmanage_window(window_id);
        close_window(window_id);
        return;
    }

    if (event->type == ewmh_connection->_NET_WM_STATE) {
        if (!window_is_managed(window_id))
            return;
        window_t *window = get_window_by_id(window_id);

        xcb_ewmh_wm_state_action_t state_action = event->data.data32[0];
        xcb_atom_t attribute = event->data.data32[1];

        if (attribute == ewmh_connection->_NET_WM_STATE_FULLSCREEN) {
            if (state_action == XCB_EWMH_WM_STATE_TOGGLE)
                window->fullscreen = !window->fullscreen;
            else
                window->fullscreen = state_action;

            monitor_t *monitor = window->monitor;

            if (window->fullscreen) {
                change_window_geometry(window->parent,
                    0, 0,
                    (unsigned int)monitor->geometry->height,
                    (unsigned int)monitor->geometry->width
                );
            } else
                set_window_geometry(window, window->geometry);

            apply();
        }

        return;
    }

}

void handle_window_click(xcb_generic_event_t *generic_event) {
    xcb_button_press_event_t *event;
    event = (xcb_button_press_event_t*)generic_event;

    xcb_window_t window_id = event->event;
    log_debug("Window(%08x) clicked", window_id);

    window_t *window;
    if (windows) {
        while ((window = vector_iterator(windows))) {
            // Redirect border window click as necessary
            if (window->parent == window_id) {
                window_id = window->id;
                break;
            }
        }

        reset_vector_iterator(windows);
    }

    if (focused_window == window_id)
        return;

    xcb_window_t previous_window = focused_window;
    focused_window = window_id;

    /* Is the newly focused window managed? */
    if (window_is_managed(focused_window)) {
        window = get_window_by_id(focused_window);
        xcb_ungrab_button(xcb_connection,
            XCB_BUTTON_INDEX_ANY, window_id, XCB_MOD_MASK_ANY);
        raise_window(window->id);
        decorate(window);
    } else {
        raise_window(window_id);
    }
    focus_window(window_id);

    /* Is the previously focused window managed? */
    if (window_is_managed(previous_window)) {
        window = get_window_by_id(previous_window);
        xcb_grab_button(xcb_connection, 0, previous_window,
            XCB_EVENT_MASK_BUTTON_PRESS,
            XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
            XCB_NONE, XCB_NONE,  XCB_BUTTON_INDEX_ANY, XCB_MOD_MASK_ANY);
        decorate(window);
    }

    apply();
}

/* Signal handlers */

void handle_termination_signal(int signal) {
    if (signal == SIGINT)
        log_message("SIGINT received");
    else
        log_message("SIGTERM received");

    custard_is_running = 0;

    /* The window manager won't die until the next iteration of the loop,
     * so let's just force it to close. */

    finalize();
    exit(EXIT_SUCCESS);
}

// TODO: handle more signals?
