#ifndef XRANDR_H
#define XRANDR_H

#include <xcb/randr.h>

unsigned short xrandr_is_available(void);
xcb_randr_get_monitors_reply_t* get_xrandr_outputs(void);

#endif
