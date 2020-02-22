#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "custard.h"
#include "config.h"
#include "decorations.h"
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
#include "../xcb/window.h"
#include "../xcb/xrandr.h"

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

    log_debug("Initializing window manager connections");
    if (!initialize()) {
        log_fatal("Initialization of window manager failed");
        finalize();
        return EXIT_FAILURE;
    }


    if (rc_path) {
        log_debug("Executing provided rc path");
        if (fork() == 0)
            execl(rc_path, rc_path, NULL);
    }

    custard_is_running = 1;
    manage_pre_existing_windows();

    xcb_generic_event_t *xcb_event;
    unsigned int xcb_event_type;

    struct pollfd descriptors[2] = {
        { xcb_file_descriptor,    .events = POLLIN },
        { socket_file_descriptor, .events = POLLIN }
    };

    log_debug("Starting event loop");
    while (custard_is_running) {

        if (poll(descriptors, 2, -1)) {
            if (descriptors[0].revents & POLLIN) {
                while ((xcb_event = xcb_poll_for_event(xcb_connection))) {
                    xcb_event_type = xcb_event->response_type & ~0x80;

                    if (xcb_events[xcb_event_type])
                        xcb_events[xcb_event_type](xcb_event);

                }
            }

            if (descriptors[1].revents & POLLIN)
                ipc_process_input(read_from_socket());
        }
    }

    finalize();

    return EXIT_SUCCESS;
}

unsigned short initialize() {
    if (!initialize_xcb() || !initialize_ewmh() || !initialize_socket())
        return 0;
    log_debug("XCB, EWMH, and socket setup");

    setup_monitors();
    log_debug("Monitors setup");
    setup_global_configuration();
    log_debug("Global configuration setup");

    unsigned int index = 0;
    for (; index < SIGUNUSED; index++)
        if (signals[index])
            signal(index, signals[index]);
    index = 0;
    log_debug("Signal handlers setup");

    return 1;
}

void manage_pre_existing_windows() {
    unsigned int index = 0;
    xcb_query_tree_cookie_t query_cookie;
    query_cookie = xcb_query_tree(xcb_connection,
        xcb_screen->root);

    xcb_query_tree_reply_t *tree_reply;
    tree_reply = xcb_query_tree_reply(xcb_connection, query_cookie, NULL);

    if (!tree_reply)
        return;

    window_t *window = NULL;
    xcb_window_t child;
    xcb_window_t *children = xcb_query_tree_children(tree_reply);
    unsigned int number_of_children = (unsigned int)
        xcb_query_tree_children_length(tree_reply);

    for (; index < number_of_children; index++) {
        child = children[index];

        if (window_should_be_managed(child)) {
            window = manage_window(child);
            map_window(child);
            raise_window(window->parent);
            focus_on_window(window);
            decorate(window);
            log_debug("Pre-existing window(%08x) managed",
                child, window->workspace);
        }
    }

    apply();
}

void finalize() {
    /* Free used memory for windows */
    if (windows) {
        log_debug("Freeing memory for windows");
        window_t *window;
        while ((window = vector_iterator(windows)))
            unmanage_window(window->id);
        deconstruct_vector(windows);
    }

    kv_pair_t *kv_pair;
    monitor_t *monitor;
    labeled_grid_geometry_t *labeled_geometry;
    if (monitors) {
        log_debug("Freeing memory for monitors");
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
        log_debug("Freeing memory for configuration");
        while ((kv_pair = vector_iterator(configuration))) {
            free(kv_pair->key);
            free(kv_pair->value);
            free(kv_pair);
        }
        deconstruct_vector(configuration);
    }

    /* Free rules, if any */

    if (rules) {
        log_debug("Freeing memory for rules");
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

    /*
     * Loglevels:
     * 0 - Absolutely fucking nothing
     * 1 - Fatal
     * 2 - Message
     * 3 - Debug
     */

     if (loglevel < level)
        return;

    fprintf(stderr, "%s:%s L%d: ", file, function, line);

    va_list ap;
    va_start(ap, formatting);
    vfprintf(stderr, formatting, ap);
    va_end(ap);

    fputs("\n", stderr);
}
