#include <stdio.h>
#include <stdlib.h>

#include "connection.h"

#include "../wm/custard.h"

xcb_connection_t* xcb_connection;
xcb_screen_t* xcb_screen;
int xcb_file_descriptor;

unsigned short initialize_xcb() {
    xcb_connection = xcb_connect(NULL, NULL);

    if (xcb_connection_has_error(xcb_connection)) return 0;

    xcb_screen = xcb_setup_roots_iterator(xcb_get_setup(xcb_connection)).data;

    unsigned int events[1] = {
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY   |
        XCB_EVENT_MASK_BUTTON_PRESS          |
        XCB_EVENT_MASK_ENTER_WINDOW          |
        XCB_EVENT_MASK_PROPERTY_CHANGE
    };

    xcb_void_cookie_t window_attributes_cookie;
    window_attributes_cookie = xcb_change_window_attributes(xcb_connection,
        xcb_screen->root, XCB_CW_EVENT_MASK, events);

    if (xcb_request_check(xcb_connection, window_attributes_cookie)) {
        log("It appears another window manager is running.");
        return 0;
    }

    events[0] ^= XCB_EVENT_MASK_ENTER_WINDOW;

    xcb_change_window_attributes_checked(xcb_connection, xcb_screen->root,
        XCB_CW_EVENT_MASK, events);

    xcb_ungrab_key(xcb_connection, XCB_GRAB_ANY, xcb_screen->root,
        XCB_MOD_MASK_ANY);

    xcb_file_descriptor = xcb_get_file_descriptor(xcb_connection);

    return 1;
}

void finalize_xcb() {
    if (xcb_connection)
        xcb_disconnect(xcb_connection);
}

void apply() {
    xcb_flush(xcb_connection);
}
