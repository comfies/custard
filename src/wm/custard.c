#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "custard.h"
#include "config.h"
#include "handlers.h"
#include "monitor.h"
#include "rules.h"
#include "window.h"

#include "../vector.h"

#include "../ipc/ipc.h"
#include "../ipc/parsing.h"
#include "../ipc/socket.h"
#include "../xcb/connection.h"
#include "../xcb/ewmh.h"
#include "../xcb/xrandr.h"
// TODO: workspaces need reimplementation

char *rc_path = NULL;
unsigned short loglevel = 0;
unsigned short custard_is_running = 0;

int custard(int argc, char **argv) {
    /* Process arguments */

    char *argument;
    for (int index = 1; index < argc; index++) {
        argument = argv[index];

        if (!strcmp(argument, "--rc")) {
            if (!argv[index + 1] || !strlen(argv[index + 1])) {
                log_fatal("No rc specified with --rc, exiting.");
                return EXIT_FAILURE;
            }

            if (access(argv[index + 1], X_OK | F_OK | R_OK) > -1) {
                rc_path = (char*)malloc(sizeof(char) *
                    (strlen(argv[index + 1]) + 1));
                strcpy(rc_path, argv[index + 1]);

                index++;
                continue;
            } else {
                log_fatal("Specified rc is not executable, exiting.");
                return EXIT_FAILURE;
            }
        }

        if (!strcmp(argument, "--loglevel")) {
            if (!argv[index + 1] || !strlen(argv[index + 1])) {
                log_fatal("No loglevel specified with --loglevel, exiting.");
                return EXIT_FAILURE;
            }

            loglevel = (short)string_to_integer(argv[index + 1]);
            index++;
            continue;
        }
    }

    /* initialize window manager */

    if (!initialize()) {
        finalize();
        return EXIT_FAILURE;
    }


    if (rc_path)
        if (fork() == 0)
            execl(rc_path, rc_path, NULL);

    custard_is_running = 1;

    xcb_generic_event_t *xcb_event;
    unsigned int xcb_event_type;

    struct pollfd descriptors[2] = {
        { xcb_file_descriptor,    .events = POLLIN },
        { socket_file_descriptor, .events = POLLIN }
    };

    while (custard_is_running) {

        if (poll(descriptors, 2, -1)) {
            if (descriptors[0].revents & POLLIN) {
                while ((xcb_event = xcb_poll_for_event(xcb_connection))) {
                    xcb_event_type = xcb_event->response_type & ~0x80;

                    if (xcb_events[xcb_event_type])
                        xcb_events[xcb_event_type](xcb_event);

                }
            }

            if (descriptors[1].revents & POLLIN) {
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

    // Potential TODO: manage pre-existing windows

    return 1;
}

void finalize() {
    /* Free used memory for windows */
    if (windows) {
        window_t *window;
        while ((window = vector_iterator(windows)))
            unmanage_window(window->id);
        deconstruct_vector(windows);
    }

    kv_pair_t *kv_pair;
    monitor_t *monitor;
    labeled_grid_geometry_t *labeled_geometry;
    if (monitors) {
        while ((monitor = vector_iterator(monitors))) {
            free(monitor->name);
            free(monitor->geometry);

            if (monitor->geometries) {
                while ((labeled_geometry = vector_iterator(
                    monitor->geometries))) {
                    free(labeled_geometry->label);
                    free(labeled_geometry->geometry);
                    free(labeled_geometry);
                }
                deconstruct_vector(monitor->geometries);
            }

            if (monitor->configuration) {
                while ((kv_pair = vector_iterator(monitor->configuration))) {
                    free(kv_pair->key);
                    free(kv_pair->value);
                    free(kv_pair);
                }
                deconstruct_vector(monitor->configuration);
            }
        }
    }

    if (configuration) {
        while ((kv_pair = vector_iterator(configuration))) {
            free(kv_pair->key);
            free(kv_pair->value);
            free(kv_pair);
        }
        deconstruct_vector(configuration);
    }

    /* Free rules, if any */

    if (rules) {
        rule_t *rule;
        while ((rule = vector_iterator(rules))) {
            free(rule->expression);

            if (rule->rules) {
                while ((kv_pair = vector_iterator(rule->rules))) {
                    free(kv_pair->key);
                    free(kv_pair->value);
                    free(kv_pair);
                }
                deconstruct_vector(rule->rules);
            }
        }
        deconstruct_vector(rules);
    }

    finalize_xcb();
    finalize_ewmh();
    finalize_socket();
}

void _log(unsigned short level, const char *file, const char *function,
    const int line, char *formatting, ...) {

    if (level >= loglevel)
        fprintf(stderr, "%s:%s L%d: ", file, function, line);

    va_list ap;
    va_start(ap, formatting);
    vfprintf(stderr, formatting, ap);
    va_end(ap);

    fputs("\n", stderr);
}
