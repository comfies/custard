#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sys/select.h>

#include "custard.h"
#include "configuration.h"
#include "ewmh.h"
#include "grid.h"
#include "handlers.h"
#include "monitor.h"
#include "rules.h"
#include "socket.h"
#include "vector.h"
#include "workspaces.h"
#include "xcb.h"

unsigned short window_manager_is_running = 0;

vector_t *configuration = NULL;
vector_t *managed_windows = NULL;
vector_t *named_geometries = NULL;
vector_t *window_rules = NULL;
vector_t *workspaces = NULL;
vector_t *monitors = NULL;

xcb_window_t focused_window = XCB_WINDOW_NONE;
unsigned int focused_workspace = 1;

/* configuration variables */

unsigned short debug_mode = 0;
char *rc_file;

/* end configuration variables */

int start_custard() {
    socket_mode = WINDOW_MANAGER;

    /* Configuration */

    configuration = construct_vector();

    append_setting(configuration, "border.color.focused",    0xFFFFFFFF);
    append_setting(configuration, "border.color.unfocused" , 0xFF676767);
    append_setting(configuration, "border.color.background", 0xFF000000);

    append_setting(configuration, "border.inner.size",   0);
    append_setting(configuration, "border.outer.size",   0);
    append_setting(configuration, "border.total.size",   0);
    append_setting(configuration, "border.color.switch", 0);
    append_setting(configuration, "border.type",         0);

    append_setting(configuration, "grid.rows",          2);
    append_setting(configuration, "grid.columns",       2);
    append_setting(configuration, "grid.offset.top",    0);
    append_setting(configuration, "grid.offset.bottom", 0);
    append_setting(configuration, "grid.offset.left",   0);
    append_setting(configuration, "grid.offset.right",  0);
    append_setting(configuration, "grid.gap",           0);
    append_setting(configuration, "workspaces",         0);

    if (!initialize_xcb_connection() || !initialize_ewmh_connection() ||
        !initialize_socket())
        return EXIT_FAILURE;

    window_manager_is_running = 1;

    managed_windows = construct_vector();
    named_geometries = construct_vector();
    window_rules = construct_vector();
    workspaces = construct_vector();
    monitors = construct_vector();

    initialize_monitors();

    if (rc_file && strlen(rc_file)) {
        debug_output("Executing specified rc file");
        if (fork() == 0)
            execl(rc_file, rc_file, NULL);
    }

    commit();

    signal(SIGTERM, signal_handler);

    xcb_generic_event_t *event;

    fd_set descriptor_set;
    int max_file_descriptor;

    while (window_manager_is_running) {
        FD_ZERO(&descriptor_set);

        FD_SET(xcb_file_descriptor, &descriptor_set);
        FD_SET(socket_file_descriptor, &descriptor_set);

        if (xcb_file_descriptor > socket_file_descriptor)
            max_file_descriptor = xcb_file_descriptor;
        else
            max_file_descriptor = socket_file_descriptor;

        max_file_descriptor++;

        if (select(max_file_descriptor, &descriptor_set, NULL, NULL, NULL)) {

            if (FD_ISSET(xcb_file_descriptor, &descriptor_set)) {
                while ((event = xcb_poll_for_event(xcb_connection))) {
                    if (event)
                        handle_event(event);
                    else {
                        stop_custard();

                        debug_output("Loop stopped during event poll");
                        return EXIT_FAILURE;
                    }
                }
            }

            if (FD_ISSET(socket_file_descriptor, &descriptor_set))
                read_from_socket();

        }
    }

    stop_custard();

    return EXIT_SUCCESS;
}

void stop_custard() {
    commit();

    unsigned int index = 0;

    for (; index < managed_windows->size; index++)
        free(get_from_vector(managed_windows, index));

    deconstruct_vector(managed_windows);

    index = 0;
    named_geometry_t *geometry = NULL;

    for (; index < named_geometries->size; index++) {
        geometry = get_from_vector(named_geometries, index);
        free(geometry->name);
        free(geometry);

    }

    deconstruct_vector(named_geometries);

    index = 0;
    window_rule_t *rule = NULL;
    setting_t *setting = NULL;

    for (; index < window_rules->size; index++) {
        rule = get_from_vector(window_rules, index);
        free(rule->expression);

        unsigned int r_index = 0;
        for (; r_index < rule->ruleset->size; r_index++) {
            setting = get_from_vector(rule->ruleset, r_index);
            free(setting->name);

            if (setting->value)
                free(setting->value);
        }

        free(rule);
    }

    deconstruct_vector(window_rules); // fuck it man I'll fix the segfault later

    index = 0;

    for (; index < workspaces->size; index++)
        free(get_from_vector(workspaces, index));

    deconstruct_vector(workspaces);

    index = 0;
    monitor_t *monitor = NULL;

    for (; index < monitors->size; index++) {
        monitor = get_from_vector(monitors, index);
        free(monitor->name);
        free(monitor->grid);
        free(monitor);
    }

    deconstruct_vector(monitors);

    index = 0;
    setting = NULL;

    for (; index < configuration->size; index++) {
        setting = get_from_vector(configuration, index);

        free(setting->name);

        if (setting->value)
            free(setting->value);
    }

    deconstruct_vector(configuration);

    finalize_socket();
    finalize_ewmh_connection();
    finalize_xcb_connection();

    if (rc_file)
        free(rc_file);

    return;
}

void signal_handler(int signal) {
    switch (signal) {
        case SIGTERM:
            window_manager_is_running = 0;
            stop_custard();
            exit(EXIT_SUCCESS);

            break;
    }
}

void _debug_output(const char *file, const char *method, const int line,
    char *formatting, ...) {

    if (!debug_mode)
        return;

    fprintf(stderr, "%-16s :%-4d (%s) - ", file, line, method);
    va_list ap;
    va_start(ap, formatting);
    vfprintf(stderr, formatting, ap);
    va_end(ap);
    fputs("\n", stderr);
}
