#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sys/select.h>

#include "custard.h"
#include "ewmh.h"
#include "grid.h"
#include "handlers.h"
#include "rules.h"
#include "socket.h"
#include "vector.h"
#include "workspaces.h"
#include "xcb.h"

unsigned short window_manager_is_running = 0;

vector_t *managed_windows = NULL;
vector_t *named_geometries = NULL;
vector_t *window_rules = NULL;
vector_t *workspaces = NULL;

xcb_window_t focused_window = XCB_WINDOW_NONE;
unsigned int focused_workspace = 1;

/* configuration variables */
unsigned short debug_mode = 0;
char *rc_file;

unsigned short border_type = 0;
unsigned short border_invert_colors = 0;

unsigned int border_inner_size = 0;
unsigned int border_outer_size = 0;
unsigned int border_total_size = 0;

unsigned int border_focused_color = 0xffffffff;
unsigned int border_unfocused_color = 0xff676767;
unsigned int border_background_color = 0xff000000;

unsigned int grid_rows = 2;
unsigned int grid_columns = 2;

unsigned int grid_gap = 0;
unsigned int grid_offset_top = 0;
unsigned int grid_offset_bottom = 0;
unsigned int grid_offset_left = 0;
unsigned int grid_offset_right = 0;

unsigned int number_of_workspaces = 1;
/* end configuration variables */

int start_custard() {
    socket_mode = WINDOW_MANAGER;

    if (!initialize_xcb_connection() || !initialize_ewmh_connection() ||
        !initialize_socket())
        return EXIT_FAILURE;

    window_manager_is_running = 1;

    managed_windows = construct_vector();
    named_geometries = construct_vector();
    window_rules = construct_vector();
    workspaces = construct_vector();

    if (rc_file && strlen(rc_file)) {
        debug_output("Executing specified rc file");
        if (fork() == 0)
            execl(rc_file, rc_file, NULL);
    }

    apply_configuration_to_grid();

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

    for (; index < window_rules->size; index++) {
        rule = get_from_vector(window_rules, index);
        free(rule->expression);
        if (rule->named_geometry)
            free(rule->named_geometry);
    }

    deconstruct_vector(window_rules);

    index = 0;

    for (; index < workspaces->size; index++)
        free(get_from_vector(workspaces, index));

    deconstruct_vector(workspaces);

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

