#ifndef HANDLERS_H
#define HANDLERS_H

#include <xcb/xcb.h>

void (*event_handlers[XCB_NO_OPERATION + 1])(xcb_generic_event_t*);

void handle_event(xcb_generic_event_t*);
void event_map_request(xcb_generic_event_t*);
void event_window_destroyed(xcb_generic_event_t*);
void event_window_clicked(xcb_generic_event_t*);
void event_message_received(xcb_generic_event_t*);

#endif /* HANDLERS_H */
