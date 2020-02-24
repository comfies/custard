#pragma once

#include <xcb/xcb.h>

#include "config.h"
#include "geometry.h"
#include "monitor.h"
#include "rules.h"

extern vector_t *windows;
extern xcb_window_t focused_window;

typedef struct {
    xcb_window_t id;
    xcb_window_t parent;
    void *geometry;
    rule_t *rule;
    monitor_t *monitor;
    unsigned short fullscreen;
    unsigned short floating;
    unsigned int workspace;
} window_t;

unsigned short window_should_be_managed(xcb_window_t);
unsigned short window_is_managed(xcb_window_t);
window_t *get_window_by_id(xcb_window_t);

window_t *manage_window(xcb_window_t);
void unmanage_window(xcb_window_t);

void set_window_geometry(window_t*, void*);
void focus_on_window(window_t*);

kv_value_t *get_setting_from_window_rules(window_t*, char*);
