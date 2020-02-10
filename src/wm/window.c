#include <string.h>

#include "config.h"
#include "custard.h"
#include "decorations.h"
#include "geometry.h"
#include "grid.h"
#include "monitor.h"
#include "rules.h"
#include "window.h"

#include "../vector.h"

#include "../xcb/connection.h"
#include "../xcb/ewmh.h"
#include "../xcb/window.h"

vector_t* windows = NULL;
xcb_window_t focused_window = XCB_WINDOW_NONE;

unsigned short window_should_be_managed(xcb_window_t window_id) {
    if (window_is_managed(window_id)) return 0;

    xcb_get_window_attributes_cookie_t window_attributes_cookie;
    window_attributes_cookie = xcb_get_window_attributes(xcb_connection,
        window_id);

    xcb_get_window_attributes_reply_t* attributes;
    attributes = xcb_get_window_attributes_reply(xcb_connection,
        window_attributes_cookie, NULL);

    if (attributes && attributes->override_redirect) return 0;

    xcb_ewmh_get_atoms_reply_t window_type;
    xcb_get_property_cookie_t window_type_cookie;
    window_type_cookie = xcb_ewmh_get_wm_window_type(ewmh_connection,
        window_id);

    if (xcb_ewmh_get_wm_window_type_reply(ewmh_connection, window_type_cookie,
        &window_type, NULL)) {
        xcb_atom_t atom;

        for (unsigned int index = 0; index < window_type.atoms_len; index++) {
            atom = window_type.atoms[index];

            if (atom == ewmh_connection->_NET_WM_WINDOW_TYPE_TOOLBAR       ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_MENU          ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_DROPDOWN_MENU ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_POPUP_MENU    ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_DND           ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_DOCK          ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_DESKTOP       ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_NOTIFICATION) {
                xcb_ewmh_get_atoms_reply_wipe(&window_type);
                return 0;
            } else if (atom == ewmh_connection->_NET_WM_WINDOW_TYPE_SPLASH) {
                xcb_ewmh_get_atoms_reply_wipe(&window_type);
                // do something else

                return 0;
            }
        }

        xcb_ewmh_get_atoms_reply_wipe(&window_type);
    }

    return 1;
}

unsigned short window_is_managed(xcb_window_t window_id) {
    window_t* window = get_window_by_id(window_id);

    if (!window)
        return 0;

    return 1;
}

window_t* get_window_by_id(xcb_window_t window_id) {
    if (window_id == xcb_screen->root || window_id == ewmh_window ||
        window_id == XCB_WINDOW_NONE)
        return NULL;

    if (windows) {
        window_t* window;
        for (unsigned int index = 0; index < windows->size; index++) {
            window = get_from_vector(windows, index);

            if (window->id == window_id)
                return window;
        }
    }

    return NULL;
}

window_t* manage_window(xcb_window_t window_id) {
    window_t* window = (window_t*)malloc(sizeof(window_t));
    window->id = window_id;
    window->parent = xcb_generate_id(xcb_connection);

    unsigned int index = 0;

    window->rule = NULL;
    if (rules) {
        rule_t* rule;
        for (; index < rules->size; index++) {
            rule = get_from_vector(rules, index);

            char* subject = NULL;
            switch (rule->attribute) {
                case class:
                    subject = class_of_window(window_id);
                    break;
                case name:
                    subject = name_of_window(window_id);
                    break;
                default:
                    index = rules->size;
                    break; // Unimplemented, somehow, return
            }

            if (expression_matches(rule->expression, subject)) {
                window->rule = rule;
                break;
            }
        }
        index = 0;
    };

    grid_geometry_t* geometry = NULL;
    monitor_t* monitor = monitor_with_cursor_residence();

    if (window->rule) {
        if (window->rule->rules) {
            kv_value_t* value;
            value = get_value_from_key(window->rule->rules, "monitor");

            if (value)
                if (monitor_from_name(value->string))
                    monitor = monitor_from_name(value->string);

            value = get_value_from_key(window->rule->rules, "geometry");
            if (value)
                geometry = get_geometry_from_monitor(monitor, value->string);
        }
    }

    if (!geometry) {
        geometry = (grid_geometry_t*)malloc(sizeof(grid_geometry_t));

        geometry->x = calculate_default_x(monitor);
        geometry->y = calculate_default_y(monitor);
        geometry->height = calculate_default_height(monitor);
        geometry->width = calculate_default_width(monitor);
    }

    window->monitor = monitor;

    /* Parent window creation */

    unsigned int values[] = {
        0, 0, 1, screen_colormap
    };
    unsigned int masked_values = XCB_CW_BACK_PIXEL |
        XCB_CW_BORDER_PIXEL | XCB_CW_OVERRIDE_REDIRECT | XCB_CW_COLORMAP;

    xcb_create_window(xcb_connection, 32,
        window->parent, xcb_screen->root,
        0, 0, 1, 1,
        0 /* border size */,
        XCB_WINDOW_CLASS_INPUT_OUTPUT, screen_visual->visual_id,
        masked_values, values);

    values[0] = XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
    masked_values = XCB_CW_EVENT_MASK;

    xcb_change_window_attributes(xcb_connection,
        window->parent, masked_values, values);

    /* Finalization */

    if (!windows)
        windows = construct_vector();

    xcb_reparent_window(xcb_connection,
        window_id, window->parent, 0, 0);
    map_window(window->parent);
    set_window_geometry(window, geometry);
    push_to_vector(windows, window);

    log("Window(%08x) managed", window_id);

    return window;
}

void unmanage_window(xcb_window_t window_id) {
    window_t* window;
    for (unsigned int index = 0; index < windows->size; index++) {
        window = get_from_vector(windows, index);

        if (window->id == window_id) {

            pull_from_vector(windows, index);
            xcb_destroy_window(xcb_connection, window->parent);
            free(window);

            log("Window(%08x) unmanaged", window_id);
            return;
        }
    }
}

void set_window_geometry(window_t* window, grid_geometry_t* geometry) {
    window->geometry = geometry;

    monitor_t* monitor = monitor_with_cursor_residence();
    // test for monitor rule here

    screen_geometry_t* screen_geometry;
    screen_geometry = get_equivalent_screen_geometry(geometry, monitor);
    apply_decoration_to_window_screen_geometry(screen_geometry);

    change_window_geometry(window->id,
        0, 0,
        (unsigned int)screen_geometry->height,
        (unsigned int)screen_geometry->width);

    change_window_geometry(window->parent,
        (unsigned int)screen_geometry->x,
        (unsigned int)screen_geometry->y,
        (unsigned int)screen_geometry->height,
        (unsigned int)screen_geometry->width);
    free(screen_geometry);

    log("Window(%08x) window geometry set", window->id);
}

void focus_on_window(window_t* window) {
    if (focused_window == window->id)
        return;

    xcb_window_t previous_window = focused_window;
    focused_window = XCB_WINDOW_NONE;

    if (window_is_managed(previous_window)) {
        xcb_grab_button(xcb_connection, 0, previous_window,
            XCB_EVENT_MASK_BUTTON_PRESS,
            XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
            XCB_NONE, XCB_NONE,
            XCB_BUTTON_INDEX_ANY, XCB_MOD_MASK_ANY);
        decorate(get_window_by_id(previous_window));
    }

    focused_window = window->id;
    xcb_ungrab_button(xcb_connection,
        XCB_BUTTON_INDEX_ANY, window->id, XCB_MOD_MASK_ANY);
    focus_window(window->id);
    decorate(window);

    log("Window(%08x) focused in place of Window(%08x)",
        window->id, previous_window);
}
