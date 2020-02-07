#ifndef XCB_CONNECTION_H
#define XCB_CONNECTION_H

#include <xcb/xcb.h>

extern xcb_connection_t* xcb_connection;
extern xcb_screen_t* xcb_screen;
extern xcb_visualtype_t* screen_visual;
extern xcb_colormap_t screen_colormap;
extern int xcb_file_descriptor;

unsigned short initialize_xcb(void);
void finalize_xcb(void);
void apply(void);

#endif
