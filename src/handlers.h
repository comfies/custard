#ifndef HANDLERS_H
#define HANDLERS_H

#include <xcb/xcb.h>

void handlers_handle_event(xcb_generic_event_t*);
void handlers_map_request(xcb_generic_event_t*);
void handlers_window_destroyed(xcb_generic_event_t*);
void handlers_button_press(xcb_generic_event_t*);
void handlers_window_message_received(xcb_generic_event_t*);

#endif /* HANDLERS_H */
