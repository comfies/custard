
#include <xcb/xcb.h>

#include "custard.h"
#include "ewmh.h"
#include "handlers.h"
#include "window.h"
#include "xcb.h"

void (*event_handlers[])(xcb_generic_event_t*) = {
    [XCB_MAP_REQUEST] = event_map_request,
    [XCB_DESTROY_NOTIFY] = event_window_destroyed,
    [XCB_CLIENT_MESSAGE] = event_message_received,
    [XCB_BUTTON_PRESS] = event_window_clicked,
    [XCB_NO_OPERATION] = NULL
};

void handle_event(xcb_generic_event_t *event) {
    unsigned int event_type = event->response_type & ~0x80;

    if (event_handlers[event_type])
        event_handlers[event_type](event);
}

void event_map_request(xcb_generic_event_t *generic_event) {
    debug_output("Called");

    xcb_map_request_event_t *event;
    event = (xcb_map_request_event_t *)generic_event;

    xcb_window_t window_id = event->window;

    unsigned short managed = manage_window(window_id);

    debug_output("Window manage call made");

    map_window(window_id);
    debug_output("Window mapped");

    raise_window(window_id);
    debug_output("Window raised");

    if (managed) {
        focus_on_window(window_id);
        debug_output("Window focused");
    }

    commit();
}

void event_window_destroyed(xcb_generic_event_t *generic_event) {
    xcb_destroy_notify_event_t *event;
    event = (xcb_destroy_notify_event_t *)generic_event;

    xcb_window_t window_id = event->window;

    unmanage_window(window_id);

    commit();
}

void event_window_clicked(xcb_generic_event_t *generic_event) {
    debug_output("Window click event");

    xcb_button_press_event_t *event;
    event = (xcb_button_press_event_t *)generic_event;

    xcb_window_t window_id = event->event;

    focus_on_window(window_id);
    raise_window(window_id);

    commit();
}

void event_message_received(xcb_generic_event_t *generic_event) {
    xcb_client_message_event_t *event;
    event = (xcb_client_message_event_t *)generic_event;

    xcb_window_t window_id = event->window;
    window_t *window = get_window_from_id(window_id);

    if (event->type == ewmh_connection->_NET_CLOSE_WINDOW) {
        if (window)
            unmanage_window(window_id);

    } else if (event->type == ewmh_connection->_NET_WM_STATE) {
        xcb_atom_t atom = event->data.data32[1];
        unsigned int action = event->data.data32[0];

        if (atom == ewmh_connection->_NET_WM_STATE_FULLSCREEN) {
            if (action == XCB_EWMH_WM_STATE_ADD) {
                //
            } else if (action == XCB_EWMH_WM_STATE_REMOVE) {
                //
            } else if (action == XCB_EWMH_WM_STATE_TOGGLE) {
                //
            }
        }
    }

    commit();
}
