#ifndef XCB_H
#define XCB_H

#include <xcb/xcb.h>

extern xcb_connection_t *xcb_connection;
extern xcb_screen_t *screen;

void commit(void);
unsigned short int initialize_xcb_connection(void);
unsigned short int finalize_xcb_connection(void);

#endif /* XCB_H */
