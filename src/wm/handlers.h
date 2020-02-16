#pragma once

#include <xcb/xcb.h>
#include <signal.h>

#ifndef SIGUNUSED
#define SIGUNUSED SIGSYS
#endif

void (*xcb_events[XCB_NO_OPERATION + 1])(xcb_generic_event_t*);
void (*signals[SIGUNUSED + 1])(int);

void handle_map_request(xcb_generic_event_t*);
void handle_window_close(xcb_generic_event_t*);
void handle_window_click(xcb_generic_event_t*);
void handle_termination_signal(int);
