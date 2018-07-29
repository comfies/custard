#include "handlers.h"

#include "custard.h"
#include "grid.h"
#include "group.h"
#include "window.h"
#include "xcb.h"
#include "ewmh.h"

#include <xcb/xcb.h>

void
handlers_handle_event(xcb_generic_event_t *event)
{
    unsigned int event_type = event->response_type & ~0x80;

    debug_output("handlers_handle_event(): event received");

    switch (event_type) {
        case (XCB_MAP_REQUEST):
            handlers_map_request(event);
            break;
        case (XCB_DESTROY_NOTIFY):
            handlers_window_destroyed(event);
            break;
        case (XCB_BUTTON_PRESS):
            handlers_button_press(event);
            break;
        case (XCB_CLIENT_MESSAGE):
            handlers_window_message_received(event);
            break;
        default:
            return;
            break;
    }

    /* TODO: create setting for sloppy focus */
}

void
handlers_map_request(xcb_generic_event_t *generic_event)
{
    debug_output("handlers_map_request(): called");

    xcb_map_request_event_t *event = (xcb_map_request_event_t *)generic_event;
    xcb_window_t window_id = event->window;

    if (window_id == ewmh_window_id) {
        return;
    }

    unsigned short int managed = manage_window(window_id);

    if (managed) {
        move_window_to_grid_coordinate(window_id, grid_window_default_x,
            grid_window_default_y);
        resize_window_with_grid_units(window_id, grid_window_default_height,
            grid_window_default_width);
        attach_window_to_group(window_id, focused_group);
        focus_on_window(window_id);
    }

    map_window(window_id);

    raise_window(window_id);
    commit();
}

void
handlers_window_destroyed(xcb_generic_event_t *generic_event)
{
    debug_output("handlers_window_destroyed(): called");

    xcb_destroy_notify_event_t *event;
    event = (xcb_destroy_notify_event_t *)generic_event;

    xcb_window_t window_id = event->window;

    if (window_list_get_window(window_id)) {
        unmanage_window(window_id);
        commit();
    }
}

void
handlers_button_press(xcb_generic_event_t *generic_event)
{
    debug_output("handlers_button_press(): called");

    xcb_button_press_event_t *event;
    event = (xcb_button_press_event_t *)generic_event;

    xcb_window_t window_id = event->event;

    focus_on_window(window_id);
    raise_window(window_id);
    commit();
}

void
handlers_window_message_received(xcb_generic_event_t *generic_event)
{
    debug_output("handlers_window_message_received(): called");

    xcb_client_message_event_t *event;
    event = (xcb_client_message_event_t *)generic_event;

    xcb_window_t window_id = event->window;

    if (event->type == ewmh_connection->_NET_WM_STATE) {
        xcb_atom_t atom = event->data.data32[1];
        unsigned int action = event->data.data32[0];

        if (atom == ewmh_connection->_NET_WM_STATE_FULLSCREEN) {

            if (action == XCB_EWMH_WM_STATE_ADD) {
                fullscreen(window_id);
            } else if (action == XCB_EWMH_WM_STATE_REMOVE) {
                debug_output("WANTING TO MAKE NORMAL AGAIN DADDY");
            } else if (action == XCB_EWMH_WM_STATE_TOGGLE) {
                debug_output("WANTING THE OPPOSITE, DADDY!");
            }

        }

    }
    commit();
}
