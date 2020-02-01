#ifndef CUSTARD_H
#define CUSTARD_H

#include <xcb/xcb.h>

#include "vector.h"

extern unsigned short window_manager_is_running;

extern vector_t *configuration;
extern vector_t *managed_windows;
extern vector_t *named_geometries;
extern vector_t *window_rules;
extern vector_t *workspaces;
extern vector_t *monitors;

extern xcb_window_t focused_window;
extern unsigned int focused_workspace;

extern unsigned short debug_mode;
extern char *rc_file;

int start_custard(void);
void stop_custard(void);

void signal_handler(int);

void _debug_output(const char*, const char*, const int, char*, ...);
#define debug_output(...) \
    _debug_output(__FILE__, __func__, __LINE__, __VA_ARGS__)

#endif /* CUSTARD_H */
