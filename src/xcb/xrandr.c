#include <xcb/xcb.h>

#include "connection.h"
#include "xrandr.h"

unsigned short xrandr_is_available() {
    if (!xcb_get_extension_data(xcb_connection, &xcb_randr_id))
        return 0;

    return 1;
}

xcb_randr_get_monitors_reply_t* get_xrandr_outputs() {
    xcb_randr_get_monitors_cookie_t monitor_cookie;
    monitor_cookie = xcb_randr_get_monitors(xcb_connection, xcb_screen->root,
        XCB_NONE);

    return xcb_randr_get_monitors_reply(xcb_connection, monitor_cookie, NULL);
}
