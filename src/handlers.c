#include "handlers.h"

#include "custard.h"
#include "grid.h"
#include "group.h"
#include "window.h"
#include "xcb.h"

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

    unsigned short int managed = manage_window(window_id);

    if (managed) {
        move_window_to_grid_coordinate(window_id, grid_window_default_x,
            grid_window_default_y);
        resize_window_with_grid_units(window_id, grid_window_default_height,
            grid_window_default_width);
        attach_window_to_group(window_id, focused_group);
    }

    map_window(window_id);

    if (managed) {
        focus_on_window(window_id);
        border_update(window_id);
    }

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
