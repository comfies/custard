#include <xcb/xcb.h>

#include "config.h"
#include "grid.h"
#include "decorations.h"
#include "../xcb/connection.h"

void decorate(window_t* window) {
    unsigned int number_of_borders = get_value_from_key(configuration,
        "borders")->number;

    if (number_of_borders > 3)
        number_of_borders = 3;

    if (!number_of_borders) {
        unsigned int values[1] = { 0 };

        xcb_configure_window(xcb_connection, window->parent,
            XCB_CONFIG_WINDOW_BORDER_WIDTH, values);
    } else if (number_of_borders == 1)
        decorate_with_one_border(window);
    else
        decorate_with_multiple_borders(window, number_of_borders);
}

unsigned int determine_border_size(window_t* window) {
    unsigned int outer_size = get_value_from_key(configuration,
        "border.size.outer")->number;
    unsigned int inner_size = get_value_from_key(configuration,
        "border.size.inner")->number;

    unsigned int number_of_borders = get_value_from_key(configuration,
        "borders")->number;

    if (!outer_size)
        outer_size = 1;

    if (!inner_size)
        inner_size = 1;

    switch (number_of_borders) {
        case 1:
            return outer_size;
        case 2:
            return outer_size + inner_size;
        case 3:
            return (outer_size * 2) + inner_size;
        default:
            return 0;
    }

    return 0;
}

void apply_decoration_to_window_screen_geometry(window_t* window,
    screen_geometry_t* geometry) {

    unsigned int border_size = determine_border_size(window);

    if (border_size) {
        geometry->height -= (border_size * 2);
        geometry->width -= (border_size * 2);
    }
}

void decorate_with_one_border(window_t* window) {
    char* color_setting = "border.color.unfocused";
    if (focused_window == window->id)
        color_setting = "border.color.focused";

    unsigned int color = get_value_from_key(configuration,
        color_setting)->number;

    unsigned int values[1] = {
        determine_border_size(window)
    };

    xcb_configure_window(xcb_connection, window->parent,
        XCB_CONFIG_WINDOW_BORDER_WIDTH, values);

    values[0] = color;

    xcb_change_window_attributes(xcb_connection, window->parent,
        XCB_CW_BORDER_PIXEL, values);
}


// TODO: refactor this at some point
void decorate_with_multiple_borders(window_t* window,
    unsigned int number_of_borders) {

    xcb_get_geometry_cookie_t geometry_cookie = xcb_get_geometry(xcb_connection,
        window->parent);
    xcb_get_geometry_reply_t* geometry;

    unsigned int values[1];

    unsigned int primary_color = get_value_from_key(configuration,
        "border.color.background")->number;
    char* color_setting = "border.color.unfocused";
    if (focused_window == window->id)
        color_setting = "border.color.focused";
    unsigned int secondary_color = get_value_from_key(configuration,
        color_setting)->number;

    xcb_pixmap_t pixmap = xcb_generate_id(xcb_connection);
    xcb_gcontext_t graphics_context = xcb_generate_id(xcb_connection);

    unsigned int outer_size = get_value_from_key(configuration,
        "border.size.outer")->number;
    unsigned int inner_size = get_value_from_key(configuration,
        "border.size.inner")->number;

    unsigned int total_border_size = determine_border_size(window);

    values[0] = total_border_size;

    xcb_configure_window(xcb_connection, window->parent,
        XCB_CONFIG_WINDOW_BORDER_WIDTH, values);

    geometry = xcb_get_geometry_reply(xcb_connection, geometry_cookie, NULL);

    unsigned short height = (unsigned short)((total_border_size * 2) +
        geometry->height);
    unsigned short width = (unsigned short)((total_border_size * 2) +
        geometry->width);

    xcb_create_pixmap(xcb_connection, 32, pixmap, xcb_screen->root,
        width, height);
    xcb_create_gc(xcb_connection, graphics_context, pixmap, 0, NULL);

    xcb_rectangle_t outer_border[4] = {
        { 0, 0, width, height }
    };
    values[0] = primary_color;
    xcb_change_gc(xcb_connection, graphics_context,
        XCB_GC_FOREGROUND, values);
    xcb_poly_fill_rectangle(xcb_connection, pixmap, graphics_context,
        4, outer_border);

    values[0] = secondary_color;
    xcb_change_gc(xcb_connection, graphics_context,
        XCB_GC_FOREGROUND, values);

    if (number_of_borders == 2) {
        xcb_rectangle_t inner_border[5] = {
            {
                (short)geometry->width, 0,
                (unsigned short)inner_size,
                (unsigned short)(geometry->height + inner_size)
            },
            {
                0, (short)geometry->height,
                (unsigned short)(geometry->width + inner_size),
                (unsigned short)inner_size
            },
            {
                (short)(geometry->width + total_border_size + outer_size), 0,
                (unsigned short)inner_size,
                (unsigned short)(geometry->height + inner_size)
            },
            {
                0, (short)(geometry->height + total_border_size + outer_size),
                (unsigned short)(geometry->width + inner_size),
                (unsigned short)inner_size
            },
            {
                (short)(geometry->width + total_border_size + outer_size),
                (short)(geometry->height + total_border_size + outer_size),
                (unsigned short)inner_size, (unsigned short)inner_size
            }
        };

        xcb_poly_fill_rectangle(xcb_connection, pixmap, graphics_context,
            5, inner_border);
    } else {
        xcb_rectangle_t inner_border[8] = {
            {
                (short)(geometry->width + outer_size), 0,
                (unsigned short)inner_size,
                (unsigned short)(geometry->height + outer_size + inner_size)
            },
            {
                0, (short)(geometry->height + outer_size),
                (unsigned short)(geometry->width + outer_size),
                (unsigned short)inner_size
            },
            {
                (short)(geometry->width + total_border_size + outer_size), 0,
                (unsigned short)inner_size,
                (unsigned short)(geometry->height + outer_size)
            },
            {
                0, (short)(geometry->height + total_border_size + outer_size),
                (unsigned short)(geometry->width + outer_size),
                (unsigned short)inner_size
            },
            {
                (short)(geometry->width + total_border_size + outer_size),
                (short)(geometry->height + total_border_size + outer_size),
                (unsigned short)inner_size,
                (unsigned short)(outer_size + inner_size)
            },
            {
                (short)(geometry->width + total_border_size + outer_size),
                (short)(geometry->height + total_border_size + outer_size),
                (unsigned short)(outer_size + inner_size),
                (unsigned short)inner_size
            },
            {
                (short)(geometry->width + outer_size),
                (short)(geometry->height + total_border_size + outer_size),
                (unsigned short)inner_size,
                (unsigned short)(outer_size + inner_size)
            },
            {
                (short)(geometry->width + total_border_size + outer_size),
                (short)(geometry->height + outer_size),
                (unsigned short)(outer_size + inner_size),
                (unsigned short)inner_size
            }
        };

        xcb_poly_fill_rectangle(xcb_connection, pixmap, graphics_context,
            8, inner_border);

        values[0] = primary_color;
    }

    xcb_change_window_attributes(xcb_connection, window->parent,
        XCB_CW_BACK_PIXEL, values);

    values[0] = pixmap;
    xcb_change_window_attributes(xcb_connection, window->parent,
        XCB_CW_BORDER_PIXMAP, values);

    xcb_free_pixmap(xcb_connection, pixmap);
    xcb_free_gc(xcb_connection, graphics_context);

}
