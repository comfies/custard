#include <xcb/xcb.h>

#include "grid.h"
#include "decorations.h"
#include "window.h"
#include "../xcb/connection.h"
#include "../xcb/window.h"

void apply_decoration_to_window_screen_geometry(window_t *window,
    screen_geometry_t *geometry) {

    unsigned int number_of_borders = get_setting_from_window_rules(window,
        "borders")->number;

    if (!number_of_borders)
        return;

    unsigned int outer_size = get_setting_from_window_rules(window,
        "border.size.outer")->number;
    unsigned int inner_size = get_setting_from_window_rules(window,
        "border.size.inner")->number;

    if (!outer_size) outer_size = 1;
    if (!inner_size) inner_size = 1;

    unsigned int border_size;
    if (number_of_borders == 1)
        border_size = outer_size;
    else
        border_size = (outer_size * (number_of_borders - 2)) +
            inner_size;

    if (!border_size)
        return;

    geometry->height -= (border_size * 2);
    geometry->width -= (border_size * 2);
}

unsigned int get_raw_color_value(color_t color) {
    unsigned int value;

    value = (color.alpha) * 0x1000000 |
        ((color.red   * color.alpha) / 0xff) * 0x10000 |
        ((color.green * color.alpha) / 0xff) * 0x100   |
        ((color.blue  * color.alpha) / 0xff);

    return value;
}

void decorate(window_t *window) {
    // Mostly preprocessing

    unsigned int number_of_borders = get_setting_from_window_rules(window,
        "borders")->number;

    if (number_of_borders > 3)
        number_of_borders = 3;

    unsigned int values[1] = { 0 };

    if (!number_of_borders) {
        configure_window(window->parent,
            XCB_CONFIG_WINDOW_BORDER_WIDTH, values);
        return;
    }

    /* Focus */

    unsigned short window_is_focused = 0;
    if (focused_window == window->id)
        window_is_focused = 1;

    /* Color */

    unsigned short inverted_borders = get_setting_from_window_rules(window,
        "border.colors.flipped")->boolean;

    char *color_setting = "border.color.unfocused";
    if (window_is_focused)
        color_setting = "border.color.focused";

    color_t background_color = get_setting_from_window_rules(window,
        "border.color.background")->color;

    color_t primary_color = get_setting_from_window_rules(window,
        color_setting)->color;
    color_t secondary_color = background_color;

    if (inverted_borders) {
        secondary_color = primary_color;
        primary_color = background_color;
    }

    /* Size */

    unsigned int outer_size = get_setting_from_window_rules(window,
        "border.size.outer")->number;
    unsigned int inner_size = get_setting_from_window_rules(window,
        "border.size.inner")->number;

    if (!outer_size) outer_size = 1;
    if (!inner_size) inner_size = 1;

    /* Which border method? */

    if (number_of_borders == 1) {
        single_border(window->parent, outer_size, primary_color);
        return;
    }

    /* Multiborder */

    multi_border(window->parent, inner_size, outer_size,
        primary_color, secondary_color, number_of_borders);

}

void single_border(xcb_window_t window, unsigned int border_size,
    color_t color) {
    /* Single border, configure window */
    unsigned int values[1] = { border_size };
    configure_window(window, XCB_CONFIG_WINDOW_BORDER_WIDTH, values);

    values[0] = get_raw_color_value(color);
    xcb_change_window_attributes(xcb_connection, window,
        XCB_CW_BORDER_PIXEL, values);
}

void multi_border(xcb_window_t window,
    unsigned int inner_size, unsigned int outer_size,
    color_t primary_color, color_t secondary_color,
    unsigned short border_type) {

    unsigned int values[1];

    xcb_get_geometry_cookie_t geometry_cookie = xcb_get_geometry(xcb_connection,
        window);
    xcb_get_geometry_reply_t *geometry;

    xcb_pixmap_t pixmap = xcb_generate_id(xcb_connection);
    xcb_gcontext_t graphics_context = xcb_generate_id(xcb_connection);

    unsigned int border_size = (outer_size * (border_type - 2)) + inner_size;

    values[0] = border_size;
    configure_window(window, XCB_CONFIG_WINDOW_BORDER_WIDTH, values);

    geometry = xcb_get_geometry_reply(xcb_connection, geometry_cookie, NULL);

    unsigned short height = (unsigned short)((border_size * 2) +
        geometry->height);
    unsigned short width = (unsigned short)((border_size * 2) +
        geometry->width);

    xcb_create_pixmap(xcb_connection, 32, pixmap, xcb_screen->root,
        width, height);
    xcb_create_gc(xcb_connection, graphics_context, pixmap, 0, NULL);


    values[0] = get_raw_color_value(secondary_color);
    xcb_change_gc(xcb_connection, graphics_context,
        XCB_GC_FOREGROUND, values);

    xcb_rectangle_t outer_border[4] = { { 0, 0, width, height } };
    xcb_poly_fill_rectangle(xcb_connection, pixmap, graphics_context,
        4, outer_border);

    values[0] = get_raw_color_value(primary_color);
    xcb_change_gc(xcb_connection, graphics_context,
        XCB_GC_FOREGROUND, values);

    if (border_type == 2)
        double_border_transient(pixmap, graphics_context,
            inner_size, outer_size, border_size, geometry);
    else {
        triple_border_transient(pixmap, graphics_context,
            inner_size, outer_size, border_size, geometry);
        values[0] = get_raw_color_value(secondary_color);
    }

    xcb_change_window_attributes(xcb_connection, window,
        XCB_CW_BACK_PIXEL, values);

    values[0] = pixmap;
    xcb_change_window_attributes(xcb_connection, window,
        XCB_CW_BORDER_PIXMAP, values);

    xcb_free_pixmap(xcb_connection, pixmap);
    xcb_free_gc(xcb_connection, graphics_context);
}

void double_border_transient(xcb_pixmap_t pixmap,
    xcb_gcontext_t graphics_context, unsigned int inner_size,
    unsigned int outer_size, unsigned int border_size,
    xcb_get_geometry_reply_t *geometry) {

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
            (short)(geometry->width + border_size + outer_size), 0,
            (unsigned short)inner_size,
            (unsigned short)(geometry->height + inner_size)
        },
        {
            0, (short)(geometry->height + border_size + outer_size),
            (unsigned short)(geometry->width + inner_size),
            (unsigned short)inner_size
        },
        {
            (short)(geometry->width + border_size + outer_size),
            (short)(geometry->height + border_size + outer_size),
            (unsigned short)inner_size, (unsigned short)inner_size
        }
    };

    xcb_poly_fill_rectangle(xcb_connection, pixmap, graphics_context,
        5, inner_border);
}

void triple_border_transient(xcb_pixmap_t pixmap,
    xcb_gcontext_t graphics_context, unsigned int inner_size,
    unsigned int outer_size, unsigned int border_size,
    xcb_get_geometry_reply_t *geometry) {

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
            (short)(geometry->width + border_size + outer_size), 0,
            (unsigned short)inner_size,
            (unsigned short)(geometry->height + outer_size)
        },
        {
            0, (short)(geometry->height + border_size + outer_size),
            (unsigned short)(geometry->width + outer_size),
            (unsigned short)inner_size
        },
        {
            (short)(geometry->width + border_size + outer_size),
            (short)(geometry->height + border_size + outer_size),
            (unsigned short)inner_size,
            (unsigned short)(outer_size + inner_size)
        },
        {
            (short)(geometry->width + border_size + outer_size),
            (short)(geometry->height + border_size + outer_size),
            (unsigned short)(outer_size + inner_size),
            (unsigned short)inner_size
        },
        {
            (short)(geometry->width + outer_size),
            (short)(geometry->height + border_size + outer_size),
            (unsigned short)inner_size,
            (unsigned short)(outer_size + inner_size)
        },
        {
            (short)(geometry->width + border_size + outer_size),
            (short)(geometry->height + outer_size),
            (unsigned short)(outer_size + inner_size),
            (unsigned short)inner_size
        }
    };

    xcb_poly_fill_rectangle(xcb_connection, pixmap, graphics_context,
        8, inner_border);
}
