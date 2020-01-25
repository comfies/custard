#include <unistd.h>

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

#include "custard.h"
#include "xcb.h"

xcb_ewmh_connection_t *ewmh_connection;
xcb_window_t ewmh_window_id;

unsigned short initialize_ewmh_connection() {
    unsigned int data[1] = { XCB_EVENT_MASK_POINTER_MOTION };

    ewmh_window_id = xcb_generate_id(xcb_connection);

    xcb_create_window(xcb_connection, XCB_COPY_FROM_PARENT, ewmh_window_id,
        screen->root, 0, 0, screen->width_in_pixels, screen->height_in_pixels,
        0, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT,
        XCB_CW_EVENT_MASK, data);

    ewmh_connection = (xcb_ewmh_connection_t *)malloc(
        sizeof(xcb_ewmh_connection_t));

    if (!ewmh_connection) {
        debug_output("Unable to establish EWMH connection.");
        free(ewmh_connection);
        return 0;
    }

    if (!xcb_ewmh_init_atoms_replies(ewmh_connection, xcb_ewmh_init_atoms(
        xcb_connection, ewmh_connection), NULL)) {
        debug_output("Unable to initialize EWMH atoms");
        return 0;
    }

    xcb_ewmh_set_wm_pid(ewmh_connection, ewmh_window_id, getpid());
    xcb_ewmh_set_wm_name(ewmh_connection, ewmh_window_id, 7, "custard");

    xcb_ewmh_set_supporting_wm_check(ewmh_connection, screen->root,
        ewmh_window_id);

    xcb_ewmh_set_supporting_wm_check(ewmh_connection, ewmh_window_id,
        ewmh_window_id);

    xcb_atom_t supported_atoms[] = {
        ewmh_connection->_NET_WM_NAME,
        ewmh_connection->_NET_ACTIVE_WINDOW,
        ewmh_connection->_NET_SUPPORTED,
        ewmh_connection->_NET_SUPPORTING_WM_CHECK,
        ewmh_connection->_NET_WM_PID,
        ewmh_connection->_NET_WM_STATE,
        ewmh_connection->_NET_CLOSE_WINDOW,
        ewmh_connection->_NET_WM_ACTION_CLOSE,

        ewmh_connection->_NET_WM_WINDOW_TYPE,
        ewmh_connection->_NET_WM_WINDOW_TYPE_DOCK,
        ewmh_connection->_NET_WM_WINDOW_TYPE_TOOLBAR,
        ewmh_connection->_NET_WM_WINDOW_TYPE_MENU,
        ewmh_connection->_NET_WM_WINDOW_TYPE_DROPDOWN_MENU,
        ewmh_connection->_NET_WM_WINDOW_TYPE_POPUP_MENU,
        ewmh_connection->_NET_WM_WINDOW_TYPE_DIALOG,
        ewmh_connection->_NET_WM_WINDOW_TYPE_DESKTOP,
        ewmh_connection->_NET_WM_WINDOW_TYPE_SPLASH,
        ewmh_connection->_NET_WM_WINDOW_TYPE_DND,

        ewmh_connection->WM_PROTOCOLS
    };

    xcb_ewmh_set_supported(ewmh_connection, 0,
        sizeof(supported_atoms)/sizeof*(supported_atoms) + 1, supported_atoms);

    return 1;
}

void finalize_ewmh_connection() {
    if (ewmh_connection) {
        debug_output("Called");

        xcb_ewmh_connection_wipe(ewmh_connection);

        return;
    }

    debug_output("Connection already closed.");
}
