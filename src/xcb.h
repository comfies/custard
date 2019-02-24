#ifndef XCB_H
#define XCB_H

#include <xcb/xcb.h>

extern xcb_connection_t *xcb_connection;
extern xcb_screen_t *screen;
extern xcb_visualtype_t *visual;
extern xcb_colormap_t colormap;
extern int xcb_file_descriptor;

unsigned short initialize_xcb_connection(void);
void finalize_xcb_connection(void);

void commit(void);

#endif /* XCB_H */

