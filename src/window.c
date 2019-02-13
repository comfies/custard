#include "window.h"

#include "config.h"
#include "ewmh.h"
#include "grid.h"
#include "workspace.h"
#include "xcb.h"

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

short unsigned int
manage_window(xcb_window_t window_id)
{
    if (window_list_get_window(window_id) || window_id == ewmh_window_id) {
        return 0;
    }

    xcb_atom_t atom;
    xcb_ewmh_get_atoms_reply_t window_type;
    xcb_get_geometry_reply_t *geometry;
    xcb_get_window_attributes_reply_t *attributes;

    geometry = xcb_get_geometry_reply(xcb_connection, xcb_get_geometry(
            xcb_connection, window_id), NULL);

    attributes = xcb_get_window_attributes_reply(xcb_connection,
        xcb_get_window_attributes(xcb_connection, window_id), NULL);

    if (xcb_ewmh_get_wm_window_type_reply(ewmh_connection,
        xcb_ewmh_get_wm_window_type(ewmh_connection, window_id), &window_type,
        NULL)) {

        for (unsigned int index = 0; index < window_type.atoms_len; index++) {
            atom = window_type.atoms[index];

            if (atom == ewmh_connection->_NET_WM_WINDOW_TYPE_TOOLBAR ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_MENU ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_DROPDOWN_MENU ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_POPUP_MENU ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_DND ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_DOCK ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_DESKTOP ||
                atom == ewmh_connection->_NET_WM_WINDOW_TYPE_NOTIFICATION) {
                xcb_ewmh_get_atoms_reply_wipe(&window_type);
                return 0;
            } else if (atom == ewmh_connection->_NET_WM_WINDOW_TYPE_SPLASH) {
                xcb_ewmh_get_atoms_reply_wipe(&window_type);
                unsigned int position[2] = {
                    (screen->width_in_pixels -
                        (unsigned int)geometry->width) / 2,
                    (screen->height_in_pixels -
                        (unsigned int)geometry->height) / 2
                };

                xcb_configure_window(xcb_connection, window_id,
                    XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, position);
            }
        }

    }

    if (attributes) {
        if (attributes->override_redirect) {
            return 0;
        }
    }

    unsigned int data[1] = {
        XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
    };

//    xcb_change_window_attributes(xcb_connection, window_id, event_mask, data);

    xcb_grab_button(xcb_connection, 0, window_id, XCB_EVENT_MASK_BUTTON_PRESS,
        XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE, XCB_NONE,
        XCB_BUTTON_INDEX_ANY, XCB_MOD_MASK_ANY);

    Window *window = (Window *)malloc(sizeof(Window));
    window->id = window_id;
    window->parent = XCB_WINDOW_NONE;
    window->mapped = 0;
    window->fullscreen = 0;
    /* Check geometry rules for matching geometry */
    window->x = grid_window_default_x;
    window->y = grid_window_default_y;

    window->height = grid_window_default_height;
    window->width = grid_window_default_width;

    window->workspace = focused_workspace->id;

    window_list_append_window(window);

    debug_output("Window created and added to linked list");

    /* Transparent borders maybe? */

    window->parent = xcb_generate_id(xcb_connection);

    unsigned int window_data[] = {
        0xff000000, 0xff000000, 1, colormap
    };

    xcb_create_window(xcb_connection, 32, window->parent,
        screen->root,
        grid_window_default_x, grid_window_default_y,
        grid_window_default_width, grid_window_default_height,
        border_total_size,
        XCB_WINDOW_CLASS_INPUT_OUTPUT, visual->visual_id,
        XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_OVERRIDE_REDIRECT |
        XCB_CW_COLORMAP,
        window_data);

    xcb_map_window(xcb_connection, window->parent);
    xcb_reparent_window(xcb_connection, window_id, window->parent, 0, 0);
    xcb_change_window_attributes(xcb_connection, window->parent, event_mask,
        data);

    /* End test code */

    return 1;

}

short unsigned int
unmanage_window(xcb_window_t window_id)
{
    debug_output("Called");

    if (focused_window && focused_window->id == window_id) {
        focused_window = NULL;
    }

    Window *window = window_list_get_window(window_id);

    xcb_destroy_window(xcb_connection, window->parent);

    window_list_remove_window(window_id);

    free(window);

    commit();

    return 1;
}

void
unfocus_window()
{
    debug_output("Called");

    xcb_set_input_focus(xcb_connection, XCB_INPUT_FOCUS_POINTER_ROOT,
        screen->root, XCB_CURRENT_TIME);

    xcb_ewmh_set_active_window(ewmh_connection, 0, XCB_WINDOW_NONE);

    if (focused_window) {
        xcb_window_t window_id = focused_window->id;
        focused_window = NULL;

        xcb_grab_button(xcb_connection, 0, window_id,
            XCB_EVENT_MASK_BUTTON_PRESS, XCB_GRAB_MODE_ASYNC,
            XCB_GRAB_MODE_ASYNC, XCB_NONE, XCB_NONE, XCB_BUTTON_INDEX_ANY,
            XCB_MOD_MASK_ANY);

        border_update(window_id);
    }
}

void
focus_on_window(xcb_window_t window_id)
{
    debug_output("Called");

    if (focused_window) {
        if (focused_window->id != window_id) {
            unfocus_window();
        } else {
            return;
        }
    }

    unsigned int data[2] = {
        XCB_ICCCM_WM_STATE_NORMAL,
        XCB_NONE
    };

    xcb_change_property(xcb_connection, XCB_PROP_MODE_REPLACE, window_id,
        ewmh_connection->_NET_WM_STATE, ewmh_connection->_NET_WM_STATE,
        32, 2, data);

    xcb_set_input_focus(xcb_connection, XCB_INPUT_FOCUS_POINTER_ROOT,
        window_id, XCB_CURRENT_TIME);

    xcb_ewmh_set_active_window(ewmh_connection, 0, window_id);

    Window *window = window_list_get_window(window_id);

    if (window) {
        focused_window = window;
        if (focused_workspace->focused_window != window_id)
            focused_workspace->focused_window = window_id;

        xcb_ungrab_button(xcb_connection, XCB_BUTTON_INDEX_ANY, window_id,
            XCB_MOD_MASK_ANY);
        border_update(window_id);
    }
}

void
close_window(xcb_window_t window_id)
{
    debug_output("Called");

    if (window_id == screen->root) {
        return;
    }

    Window *window = window_list_get_window(window_id);

    if (window) {
        unmanage_window(window_id);
    }

    xcb_icccm_get_wm_protocols_reply_t protocols;
    xcb_icccm_get_wm_protocols_reply(xcb_connection,
        xcb_icccm_get_wm_protocols(xcb_connection, window_id,
            ewmh_connection->WM_PROTOCOLS), &protocols, NULL);

    xcb_atom_t delete_window_atom;
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(xcb_connection,
        xcb_intern_atom(xcb_connection, 0, 16, "WM_DELETE_WINDOW"), NULL);

    if (reply) {
        delete_window_atom = reply->atom;

        debug_output("Attempting to use WM protocols to close window");

        for (unsigned int index = 0; index < protocols.atoms_len; index++) {
            if (protocols.atoms[index] == delete_window_atom) {

                xcb_client_message_event_t event = {
                    .response_type = XCB_CLIENT_MESSAGE,
                    .format = 32,
                    .sequence = 0,
                    .window = window_id,
                    .type = ewmh_connection->WM_PROTOCOLS,
                    .data.data32 = {
                        delete_window_atom,
                        XCB_CURRENT_TIME
                    }
                };

                xcb_send_event(xcb_connection, 0, window_id,
                    XCB_EVENT_MASK_NO_EVENT, (char *)&event);

                xcb_icccm_get_wm_protocols_reply_wipe(&protocols);
                return;
            }
        }

    }

    xcb_kill_client(xcb_connection, window_id);

    debug_output("Window closed via xcb_kill_client(...)");

}

void
map_window(xcb_window_t window_id)
{
    Window *window = window_list_get_window(window_id);

    if (window) {
        xcb_map_window(xcb_connection, window_id);
        window_id = window->parent;
    }

    xcb_map_window(xcb_connection, window_id);
}

void
unmap_window(xcb_window_t window_id)
{
    Window *window = window_list_get_window(window_id);

    if (window) {
        xcb_unmap_window(xcb_connection, window->parent);
    } else { 
        xcb_unmap_window(xcb_connection, window_id);
    }
}

void
raise_window(xcb_window_t window_id) {
    Window *window = window_list_get_window(window_id);

    if (window) {
        window_id = window->parent;
    }

    unsigned int data[1] = {XCB_STACK_MODE_ABOVE};

    xcb_configure_window(xcb_connection, window_id,
        XCB_CONFIG_WINDOW_STACK_MODE, data);
}

void
lower_window(xcb_window_t window_id) {
    Window *window = window_list_get_window(window_id);

    if (window) {
        window_id = window->parent;
    }

    unsigned int data[1] = {XCB_STACK_MODE_BELOW};

    xcb_configure_window(xcb_connection, window_id,
        XCB_CONFIG_WINDOW_STACK_MODE, data);
}

void
change_window_geometry_pixels(xcb_window_t window_id, unsigned int x,
    unsigned int y, unsigned int height, unsigned int width)
{
    unsigned int data[4] = {x, y, width, height};

    xcb_configure_window(xcb_connection, window_id, XCB_CONFIG_WINDOW_X |
        XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_HEIGHT |
        XCB_CONFIG_WINDOW_WIDTH, data);
}

void
change_window_geometry_grid_coordinate(xcb_window_t window_id, unsigned int x,
    unsigned int y, unsigned int height, unsigned int width)
{
    Window *window = window_list_get_window(window_id);

    unsigned int x_in_pixels = grid_get_offset_x(x) + grid_margin_left;
    unsigned int y_in_pixels = grid_get_offset_y(y) + grid_margin_top;

    unsigned int height_in_pixels = grid_get_span_y(height);
    unsigned int width_in_pixels = grid_get_span_x(width);

    xcb_window_t parent = XCB_WINDOW_NONE;

    if (window)
    {
        parent = window->parent;
        window->x = x;
        window->y = y;
        window->height = height;
        window->width = width;
    }

    resize_window_with_pixels(window_id, height_in_pixels, width_in_pixels);

    if (parent != XCB_WINDOW_NONE)
        change_window_geometry_pixels(parent, x_in_pixels, y_in_pixels,
            height_in_pixels, width_in_pixels);
}

void
move_window_to_pixel_coordinate(xcb_window_t window_id, unsigned int x,
    unsigned int y)
{
    unsigned int data[2] = {x, y};

    xcb_configure_window(xcb_connection, window_id,
        XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, data);
}

void
move_window_to_grid_coordinate(xcb_window_t window_id, unsigned int x,
    unsigned int y)
{
    unsigned int x_in_pixels = grid_get_offset_x(x) + grid_margin_left;
    unsigned int y_in_pixels = grid_get_offset_y(y) + grid_margin_top;

    Window *window = window_list_get_window(window_id);

    if (window) {
        window_id = window->parent;
        window->x = x;
        window->y = y;
    }

    move_window_to_pixel_coordinate(window_id, x_in_pixels, y_in_pixels);
}

void
resize_window_with_pixels(xcb_window_t window_id, unsigned int height,
    unsigned int width)
{
    unsigned int data[2] = {width, height};

    xcb_configure_window(xcb_connection, window_id,
        XCB_CONFIG_WINDOW_HEIGHT | XCB_CONFIG_WINDOW_WIDTH, data);
}

void
resize_window_with_grid_units(xcb_window_t window_id, unsigned int height,
    unsigned int width)
{
    unsigned int height_in_pixels = grid_get_span_y(height);
    unsigned int width_in_pixels = grid_get_span_x(width);

    Window *window = window_list_get_window(window_id);

    if (window) {
        resize_window_with_pixels(window_id, height_in_pixels,
            width_in_pixels);
        window_id = window->parent;
        window->height = height;
        window->width = width;
    }

    resize_window_with_pixels(window_id, height_in_pixels, width_in_pixels);
}

void
fullscreen(xcb_window_t window_id)
{
    Window *window = window_list_get_window(window_id);

    if (!window) {
        return;
    }

    if (window->fullscreen)  {
        return;
    }

    window->fullscreen = 1;

    unsigned int border_width[1] = {0};

    xcb_configure_window(xcb_connection, window->parent,
        XCB_CONFIG_WINDOW_BORDER_WIDTH, border_width);

    move_window_to_pixel_coordinate(window->parent, 0, 0);
    resize_window_with_pixels(window->parent, screen->height_in_pixels,
        screen->width_in_pixels);
    resize_window_with_pixels(window_id, screen->height_in_pixels,
        screen->width_in_pixels);

}

void
window(xcb_window_t window_id)
{
    Window *window = window_list_get_window(window_id);

    if (!window) {
        return;
    }

    if (!window->fullscreen) {
        return;
    }

    window->fullscreen = 0;

    move_window_to_grid_coordinate(window_id, window->x, window->y);
    resize_window_with_grid_units(window_id, window->height, window->width);
    border_update(window_id);
}

void
border_update(xcb_window_t window_id)
{
    Window *window = window_list_get_window(window_id);

    if (!window || border_type == 0) {
        return;
    }

    xcb_get_geometry_reply_t *geometry = xcb_get_geometry_reply(xcb_connection,
        xcb_get_geometry(xcb_connection, window->parent), NULL);

    if (!geometry) {
        return;
    }

    unsigned int data[1] = {border_total_size};

    xcb_configure_window(xcb_connection, window->parent,
        XCB_CONFIG_WINDOW_BORDER_WIDTH, data);

    debug_output("Selecting border update method");

    unsigned int colors[2] = {
        border_unfocused_color,
        border_background_color
    };

    debug_output("Colors: %x %x %x", border_focused_color,
        border_unfocused_color, border_background_color);

    short unsigned int focused = 0;
    if (focused_window) {
        if (focused_window->id == window_id) {
            focused = 1;
        }
    }

    if (focused) {
        if (border_invert_colors) {
            colors[1] = border_focused_color;
            colors[0] = border_background_color;
        } else {
            colors[0] = border_focused_color;
        }
    } else {
        if (border_invert_colors) {
            colors[1] = border_unfocused_color;
            colors[0] = border_background_color;
        }
    }

    if (border_type == 1) {
        data[0] = colors[0];

        xcb_change_window_attributes(xcb_connection, window->parent,
            XCB_CW_BORDER_PIXEL, data);

    } else {

        short unsigned height = geometry->height + (border_total_size * 2);
        short unsigned width = geometry->width + (border_total_size * 2);

        xcb_rectangle_t outer_border[4] = {
            {0, 0, width, height}
        };

        xcb_pixmap_t pixmap = xcb_generate_id(xcb_connection);
        xcb_gcontext_t graphics_context = xcb_generate_id(xcb_connection);

        xcb_create_pixmap(xcb_connection, 32, pixmap, screen->root,
            width, height);
        xcb_create_gc(xcb_connection, graphics_context, pixmap, 0, NULL);

        data[0] = colors[1];
        xcb_change_gc(xcb_connection, graphics_context, XCB_GC_FOREGROUND,
            data);

        xcb_poly_fill_rectangle(xcb_connection, pixmap, graphics_context,
            4, outer_border);

        data[0] = colors[0];
        xcb_change_gc(xcb_connection, graphics_context, XCB_GC_FOREGROUND,
            data);

        if (border_type == 2) {
            xcb_rectangle_t double_border[5] = {

                {
                    (short)geometry->width, 0,
                    (short unsigned)border_inner_size,
                    (short unsigned)(geometry->height + border_inner_size)
                },

                {
                    0, (short)geometry->height,
                    (short unsigned)(geometry->width + border_inner_size),
                    (short unsigned)border_inner_size
                },

                {
                    (short)(geometry->width + border_total_size +
                        border_outer_size), 0,
                    (short unsigned)border_inner_size,
                    (short unsigned)(geometry->height + border_inner_size)
                },

                {
                    0, (short)(geometry->height + border_total_size +
                        border_outer_size),
                    (short unsigned)(geometry->width + border_inner_size),
                    (short unsigned)border_inner_size
                },

                {
                    (short)(geometry->width + border_total_size +
                        border_outer_size), (short)(geometry->height +
                        border_total_size +
                        border_outer_size),
                    (short unsigned)border_inner_size,
                    (short unsigned)border_inner_size
                }
            };


            xcb_poly_fill_rectangle(xcb_connection, pixmap, graphics_context,
                5, double_border);

        } else if (border_type == 3) {
            xcb_rectangle_t triple_border[8] = {

                {
                    (short)(geometry->width + border_outer_size), 0,
                    (short unsigned)border_inner_size,
                    (short unsigned)(geometry->height + border_outer_size +
                        border_inner_size)
                }, /* Right */

                {
                    0, (short)(geometry->height + border_outer_size),
                    (short unsigned)(geometry->width + border_outer_size),
                    (short unsigned)border_inner_size
                }, /* Bottom */

                {
                    (short)(geometry->width + border_total_size +
                        border_outer_size), 0,
                    (short unsigned)border_inner_size,
                    (short unsigned)(geometry->height + border_outer_size)
                }, /* Left */

                {
                    0, (short)(geometry->height + border_total_size +
                        border_outer_size),
                    (short unsigned)(geometry->width + border_outer_size),
                    (short unsigned)border_inner_size
                }, /* Top */

                {
                    (short)(geometry->width + border_total_size +
                        border_outer_size), (short)(geometry->height +
                        border_total_size +
                        border_outer_size),
                    (short unsigned)border_inner_size,
                    (short unsigned)(border_outer_size +
                        border_inner_size)
                }, /* Top-left, first fragment */

                {
                    (short)(geometry->width + border_total_size +
                        border_outer_size), (short)(geometry->height +
                        border_total_size +
                        border_outer_size),
                    (short unsigned)(border_outer_size +
                        border_inner_size),
                    (short unsigned)border_inner_size,
                }, /* Top-left, second fragment */

                {
                    (short)(geometry->width + border_outer_size), (short)(
                        geometry->height + border_total_size +
                        border_outer_size),
                    (short unsigned)border_inner_size,
                    (short unsigned)(border_inner_size +
                        border_outer_size)
                }, /* Top-right */

                {
                    (short)(geometry->width + border_total_size +
                        border_outer_size), (short)(geometry->height +
                        border_outer_size),
                    (short unsigned)(border_inner_size +
                        border_outer_size),
                    (short unsigned)border_inner_size
                } /* Bottom-left */
            };


            xcb_poly_fill_rectangle(xcb_connection, pixmap, graphics_context,
                8, triple_border);
        }

        data[0] = pixmap;
        xcb_change_window_attributes(xcb_connection, window->parent,
            XCB_CW_BORDER_PIXMAP, data);

        xcb_free_pixmap(xcb_connection, pixmap);
        xcb_free_gc(xcb_connection, graphics_context);

    }
}
