#include "custard.h"

#include "config.h"
#include "ewmh.h"
#include "handlers.h"
#include "grid.h"
#include "socket.h"
#include "window.h"
#include "xcb.h"

#include <xcb/xcb.h>

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

unsigned short debug = 0;
unsigned short wm_running = 0;
const char *config_path = NULL;

struct WindowLinkedListElement *window_list_head = NULL;
Window *focused_window;
pthread_t socket_thread;

short unsigned int
window_list_append_window(Window *window)
{
    if (window_list_get_window(window->id)) {
        return 0;
    }

    struct WindowLinkedListElement *element = window_list_head;

    if (!element) {
        debug_output("window_list_append_window(): No head, making window head");

        window_list_head = (struct WindowLinkedListElement *)malloc(
            sizeof(struct WindowLinkedListElement)
        );
        window_list_head->next = NULL;
        window_list_head->window = window;

    } else {
        debug_output("window_list_append_window(): Head found, making tail");

        while (element->next) {
            element = element->next;
        }
        
        struct WindowLinkedListElement *next_element;
        next_element = (struct WindowLinkedListElement *)malloc(
            sizeof(struct WindowLinkedListElement)
        );
        next_element->window = window;
        next_element->next = NULL;

        element->next = next_element;
    }

    return 1;
}

short unsigned int
window_list_remove_window(xcb_window_t window_id)
{
    struct WindowLinkedListElement *element = window_list_head;
    struct WindowLinkedListElement *old_element = NULL;

    if (window_list_head) {
        if (window_list_head->window->id == window_id) {
            element = window_list_head;

            window_list_head = element->next;
/*            free(element->window);*/
            free(element);

            return 1;
        }
    } else {
        return 0;
    }

    while (element) {
        if (element->next->window->id == window_id) {
            old_element = element->next;

            element->next = element->next->next;
/*            free(old_element->window);*/
            free(old_element);
            return 1;
        }

        element = element->next;
    }

    return 0;
}

Window *
window_list_get_window(xcb_window_t window_id)
{
    struct WindowLinkedListElement *last_window_element = window_list_head;

    while (last_window_element) {
        if (last_window_element->window->id == window_id) {
            return last_window_element->window;
        }

        last_window_element = last_window_element->next;
    }

    return NULL;
}

void
debug_output(char *message)
{
    if (debug) {
        fprintf(stderr, "[debug] %s\n", message);
    }
}

int
start_custard()
{
    if (!initialize_xcb_connection() ||
        !initialize_ewmh_connection() ||
        !initialize_socket()) {
        return EXIT_FAILURE;
    }

    Configuration = (struct Config *)malloc(sizeof(struct Config));
    apply_config_defaults();
    grid_apply_configuration();

    wm_running = 1;

    xcb_generic_event_t *event;

    pthread_create(&socket_thread, NULL, start_socket_read_loop, NULL);

    if (config_path) {
        if (debug) {
            fprintf(stderr, "[debug] Executing %s\n", config_path);
        }

        if (fork() == 0) {
            execl(config_path, config_path, NULL);

            // Shouldn't be reached
            fprintf(stderr, "[error] Unable to execute %s\n", config_path);
            exit(EXIT_FAILURE);
        }
    }

    xcb_query_tree_reply_t *tree_reply;
    tree_reply = xcb_query_tree_reply(
        xcb_connection,
        xcb_query_tree(
            xcb_connection,
            screen->root
        ),
        NULL
    );

    if (tree_reply) {
        xcb_window_t *tree_children;
        xcb_window_t window_id;
        unsigned short int managed;

        tree_children = xcb_query_tree_children(tree_reply);
        unsigned int length = xcb_query_tree_children_length(tree_reply);

        for (unsigned int index = 0; index < length; index++) {
            window_id = tree_children[index];
            managed = manage_window(window_id);

            if (managed) {
                move_window_to_grid_coordinate(window_id,
                    grid_window_default_x, grid_window_default_y);
                resize_window_with_grid_units(window_id,
                    grid_window_default_height, grid_window_default_width);
                map_window(window_id);
                focus_on_window(window_id);
                border_update(window_id);
                raise_window(window_id);
            }

        }

        commit();
    }

    debug_output("main(): starting event loop");

    while (wm_running) {
        event = xcb_wait_for_event(xcb_connection);
        if (event) {
            handlers_handle_event(event);
/*            free(event);*/
        } else {
            stop_custard();
        }
    }

    debug_output("main(): event loop stopped, shutting down");

    return EXIT_SUCCESS;
}

void
stop_custard()
{
    debug_output("stop_custard(): called, joining socket thread and finalizing");

    if (!wm_running) {
        return;
    }

    xcb_query_tree_reply_t *tree_reply;
    tree_reply = xcb_query_tree_reply(
        xcb_connection,
        xcb_query_tree(
            xcb_connection,
            screen->root
        ),
        NULL
    );

    if (tree_reply) {
        xcb_window_t *tree_children;
        xcb_window_t window_id;

        tree_children = xcb_query_tree_children(tree_reply);
        unsigned int length = xcb_query_tree_children_length(tree_reply);

        for (unsigned int index = 0; index < length; index++) {
            window_id = tree_children[index];
            close_window(window_id);
        }
    }

    commit();

    pthread_join(socket_thread, NULL);

    finalize_socket();
    finalize_ewmh_connection();
    finalize_xcb_connection();

    free(Configuration);

    wm_running = 0;
}

