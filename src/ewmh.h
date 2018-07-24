#ifndef EWMH_H
#define EWMH_H

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

extern xcb_ewmh_connection_t *ewmh_connection;
extern xcb_window_t ewmh_window_id;

unsigned short initialize_ewmh_connection(void);
unsigned short finalize_ewmh_connection(void);

#endif /* EWMH_H */
