#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "custard.h"
#include "config.h"
#include "handlers.h"
#include "monitor.h"
#include "rules.h"
#include "window.h"

#include "../vector.h"

#include "../ipc/ipc.h"
#include "../ipc/socket.h"
#include "../xcb/connection.h"
#include "../xcb/ewmh.h"
#include "../xcb/xrandr.h"
// TODO: workspaces need reimplementation

unsigned short custard_is_running = 0;
vector_t* windows;

int custard() {
    if (!initialize())
        return EXIT_FAILURE;

    custard_is_running = 1;

    xcb_generic_event_t *xcb_event;
    unsigned int xcb_event_type;

    fd_set descriptors;
    int descriptor;

    while (custard_is_running) {
        FD_ZERO(&descriptors);
        FD_SET(xcb_file_descriptor, &descriptors);
        FD_SET(socket_file_descriptor, &descriptors);

        descriptor = xcb_file_descriptor;
        if (xcb_file_descriptor < socket_file_descriptor)
            descriptor = socket_file_descriptor;
        descriptor++;

        if (select(descriptor, &descriptors, NULL, NULL, NULL)) {
            if (FD_ISSET(xcb_file_descriptor, &descriptors)) {

                while ((xcb_event = xcb_poll_for_event(xcb_connection))) {
                    xcb_event_type = xcb_event->response_type & ~0x80;

                    if (xcb_events[xcb_event_type])
                        xcb_events[xcb_event_type](xcb_event);

                }

            }

            if (FD_ISSET(socket_file_descriptor, &descriptors)) {
                ipc_process_input(read_from_socket());
            }
        }
    }

    finalize();

    return EXIT_SUCCESS;
}

unsigned short initialize() {
    if (!initialize_xcb() || !initialize_ewmh() || !initialize_socket())
        return 0;

    setup_monitors();
    setup_global_configuration();

    unsigned int index = 0;
    for (; index < SIGUNUSED; index++)
        if (signals[index])
            signal(index, signals[index]);
    index = 0;

    windows = construct_vector();

    // Potential TODO: manage pre-existing windows

    return 1;
}

void finalize() {
    unsigned int index = 0;
    unsigned int nested_index = 0;

    /* Free used memory for windows */
    window_t* window;
    for (; index < windows->size; index++) {
        window = get_from_vector(windows, index);
        unmanage_window(window->id);
    }
    deconstruct_vector(windows);
    index = 0;

    kv_pair_t* kv_pair;
    monitor_t* monitor;
    for (; index < monitors->size; index++) {
        monitor = get_from_vector(monitors, index);

        free(monitor->name);
        free(monitor->geometry);

        if (monitor->configuration) {
            for (; nested_index < monitor->configuration->size;
                nested_index++) {
                kv_pair = get_from_vector(monitor->configuration, index);

                free(kv_pair->key);
                free(kv_pair->value);
                free(kv_pair);
            }
            nested_index = 0;

            deconstruct_vector(monitor->configuration);
        }
    }
    nested_index = index = 0;

    for (; index < configuration->size; index++) {
        kv_pair = get_from_vector(configuration, index);

        free(kv_pair->key);
        free(kv_pair->value);
        free(kv_pair);
    }
    deconstruct_vector(configuration);
    index = 0;

    /* Free rules, if any */

    if (rules) {
        rule_t* rule;
        for (; index < rules->size; index++) {
            rule = get_from_vector(rules, index);

            free(rule->expression);

            if (rule->rules) {
                for (; nested_index < rule->rules->size; nested_index++) {
                    kv_pair = get_from_vector(rule->rules, nested_index);

                    free(kv_pair->key);
                    free(kv_pair->value);
                    free(kv_pair);
                }
                nested_index = 0;
            }

            deconstruct_vector(rule->rules);
        }

        deconstruct_vector(rules);
    }

    finalize_xcb();
    finalize_ewmh();
    finalize_socket();
}

void _log(const char* file, const char* function, const int line,
    char* formatting, ...) {

    fprintf(stderr, "%s:%s L%d: ", file, function, line);

    va_list ap;
    va_start(ap, formatting);
    vfprintf(stderr, formatting, ap);
    va_end(ap);

    fputs("\n", stderr);
}
