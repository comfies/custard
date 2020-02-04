#ifndef EWMH_H
#define EWMH_H

#include <xcb/xcb_ewmh.h>

extern xcb_ewmh_connection_t* ewmh_connection;
extern xcb_window_t ewmh_window;

unsigned short initialize_ewmh(void);
void finalize_ewmh(void);

#endif
