#include <xcb/xcb_icccm.h>

#include "connection.h"
#include "ewmh.h"
#include "window.h"

#include "../wm/custard.h"

// TODO: add focus methods

void configure_window(xcb_window_t window_id, unsigned int value_mask,
    unsigned int* values) {
    xcb_configure_window(xcb_connection, window_id, value_mask, values);
}

void map_window(xcb_window_t window_id) {
    xcb_map_window(xcb_connection, window_id);

    log("Window(%08x) mapped", window_id);
}

void unmap_window(xcb_window_t window_id) {
    xcb_unmap_window(xcb_connection, window_id);

    log("Window(%08x) unmapped", window_id);
}

void raise_window(xcb_window_t window_id) {
    unsigned int value_mask = XCB_CONFIG_WINDOW_STACK_MODE;
    unsigned int values[1] = {
        XCB_STACK_MODE_ABOVE
    };

    configure_window(window_id, value_mask, values);

    log("Window(%08x) raised", window_id);
}

void lower_window(xcb_window_t window_id) {
    unsigned int value_mask = XCB_CONFIG_WINDOW_STACK_MODE;
    unsigned int values[1] = {
        XCB_STACK_MODE_BELOW
    };

    configure_window(window_id, value_mask, values);

    log("Window(%08x) lowered", window_id);
}

void close_window(xcb_window_t window_id) {
    xcb_get_property_cookie_t protocols_cookie;
    protocols_cookie = xcb_icccm_get_wm_protocols(xcb_connection, window_id,
        ewmh_connection->WM_PROTOCOLS);

    xcb_intern_atom_cookie_t delete_cookie;
    delete_cookie = xcb_intern_atom(xcb_connection, 0, 16, "WM_DELETE_WINDOW");

    xcb_icccm_get_wm_protocols_reply_t protocols;
    xcb_icccm_get_wm_protocols_reply(xcb_connection, protocols_cookie,
        &protocols, NULL);

    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(xcb_connection,
        delete_cookie, NULL);

    if (reply) {
        for (unsigned int index = 0; index < protocols.atoms_len; index++) {
            if (protocols.atoms[index] == reply->atom) {

                xcb_client_message_event_t event = {
                    .response_type = XCB_CLIENT_MESSAGE,
                    .format        = 32,
                    .sequence      = 0,
                    .window        = window_id,
                    .type          = ewmh_connection->WM_PROTOCOLS,
                    .data.data32   = {
                        reply->atom, XCB_CURRENT_TIME
                    }
                };

                xcb_send_event(xcb_connection, 0, window_id,
                    XCB_EVENT_MASK_NO_EVENT, (char*)&event);
                xcb_icccm_get_wm_protocols_reply_wipe(&protocols);

                log("Window(%08x) closed(WM_PROTOCOLS)", window_id);
                return;
            }
        }
    }

    xcb_icccm_get_wm_protocols_reply_wipe(&protocols);
    xcb_kill_client(xcb_connection, window_id);

    log("Window(%08x) closed(xcb_kill_client)", window_id);
}

void change_window_geometry(xcb_window_t window_id, unsigned int x,
    unsigned int y, unsigned int height, unsigned int width) {

    unsigned int value_mask = XCB_CONFIG_WINDOW_X |
        XCB_CONFIG_WINDOW_Y     |
        XCB_CONFIG_WINDOW_WIDTH |
        XCB_CONFIG_WINDOW_HEIGHT;
    unsigned int values[4] = { x, y, width, height };

    configure_window(window_id, value_mask, values);

    log("Window(%08x) geometry changed %dx%d (%d,%d)", window_id,
        width, height, x, y);
}

void* get_window_property(xcb_window_t window_id, xcb_atom_t property,
    xcb_atom_t atom_type) {
    xcb_get_property_cookie_t atom_cookie;
    atom_cookie = xcb_get_property(xcb_connection, 0, window_id, property,
        atom_type, 0, 256);

    xcb_get_property_reply_t *property_cookie;
    property_cookie = xcb_get_property_reply(xcb_connection, atom_cookie, NULL);

    void* window_property;
    window_property = xcb_get_property_value(property_cookie);

    return window_property;
}

char* name_of_window(xcb_window_t window_id) {
    char* name = (char*)get_window_property(window_id, XCB_ATOM_WM_NAME,
        XCB_GET_PROPERTY_TYPE_ANY);

    return name;
}

char* class_of_window(xcb_window_t window_id) {
    char* class = (char*)get_window_property(window_id, XCB_ATOM_WM_CLASS,
        XCB_GET_PROPERTY_TYPE_ANY);

    return class;
}
