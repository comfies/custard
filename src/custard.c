#include "custard.h"

#include "config.h"
#include "ewmh.h"
#include "handlers.h"
#include "grid.h"
#include "socket.h"
#include "window.h"
#include "workspace.h"
#include "xcb.h"

#include <xcb/xcb.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/select.h>

unsigned short debug = 0;
unsigned short wm_running = 0;
const char *config_path = NULL;
int xcb_file_descriptor;

struct LinkedListElement *window_list_head = NULL;
Window *focused_window = NULL;

short unsigned int
window_list_append_window(Window *window)
{
    if (window_list_get_window(window->id)) {
        return 0;
    }

    struct LinkedListElement *element = window_list_head;

    if (!element) {
        debug_output("No head, making window head");

        window_list_head = (struct LinkedListElement *)malloc(
            sizeof(struct LinkedListElement));
        window_list_head->next = NULL;
        window_list_head->data = window;

    } else {
        debug_output("Head found, making tail");

        while (element->next) {
            element = element->next;
        }
        
        struct LinkedListElement *next_element;
        next_element = (struct LinkedListElement *)malloc(
            sizeof(struct LinkedListElement)
        );
        next_element->data = window;
        next_element->next = NULL;

        element->next = next_element;
    }

    return 1;
}

short unsigned int
window_list_remove_window(xcb_window_t window_id)
{
    struct LinkedListElement *element = window_list_head;
    struct LinkedListElement *old_element = NULL;

    if (window_list_head) {
        if (((Window *)window_list_head->data)->id == window_id) {
            element = window_list_head;

            window_list_head = element->next;
            free(element);

            return 1;
        }
    } else {
        return 0;
    }

    while (element) {
        if (((Window *)((struct LinkedListElement *)element->next)->data)->id
            == window_id) {
            old_element = element->next;

            element->next = ((struct LinkedListElement *)element->next)->next;
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
    struct LinkedListElement *last_window_element = window_list_head;

    while (last_window_element) {
        if (((Window *)last_window_element->data)->id == window_id) {
            return last_window_element->data;
        }

        last_window_element = last_window_element->next;
    }

    return NULL;
}

void
_debug(const char *file, const char *func, const int line, char *fmt, ...)
{
    if (!debug)
        return;
    fprintf(stderr, "%-14s :%-4d (%s) - ", file, line, func);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);
}

int
start_custard()
{
    if (!initialize_xcb_connection() ||
        !initialize_ewmh_connection() ||
        !initialize_socket()) {
        return EXIT_FAILURE;
    }

    initialize_configuration();

    xcb_file_descriptor = xcb_get_file_descriptor(xcb_connection);

    grid_apply_configuration();

    wm_running = 1;

    xcb_generic_event_t *event;

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
                raise_window(window_id);
                border_update(window_id);
            }

        }
        commit();
    }

    debug_output("Starting event loop");
    fd_set descriptor_set;
    int max_file_descriptor;

    while (wm_running) {
        FD_ZERO(&descriptor_set);
        FD_SET(socket_file_descriptor, &descriptor_set);
        FD_SET(xcb_file_descriptor, &descriptor_set);

        max_file_descriptor = xcb_file_descriptor;

        if (xcb_file_descriptor < socket_file_descriptor) {
            max_file_descriptor = socket_file_descriptor;
        }

        max_file_descriptor++;

        if (select(max_file_descriptor, &descriptor_set,
            NULL, NULL, NULL)> 0) {
            if (FD_ISSET(socket_file_descriptor, &descriptor_set)) {
                read_socket();
            }
            
            if (FD_ISSET(xcb_file_descriptor, &descriptor_set)) {
                while ((event = xcb_poll_for_event(xcb_connection))) {
                    if (event) {
                        debug_output("Event poll received event");
                        handlers_handle_event(event);
                    } else {
                        stop_custard();
                        debug_output("Loop stopped during iteration.");
                        return EXIT_FAILURE;
                    }
                }
            }
        }
    }

    stop_custard();

    debug_output("Event loop stopped, shutting down");

    return EXIT_SUCCESS;
}

void
stop_custard()
{
    debug_output("Ending custard");

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

    finalize_socket();
    finalize_ewmh_connection();
    finalize_xcb_connection();
    finalize_configuration();

}

void
focus_next_window()
{
    debug_output("Called");

    struct LinkedListElement *element = window_list_head;
    Window *window = NULL;
    short unsigned passed = 0;

    if (!focused_window) {
        focus_on_window(((Window *)element->data)->id);
        raise_window(((Window *)element->data)->id);
        return;
    }

    while (element) {
        window = (Window *)element->data;

        if (!passed && window->id == focused_window->id) {
            passed++;
        } else if (passed) {
            if (passed > 1) {
                return;
            }

            if (window_is_in_workspace(window, focused_workspace)) {
                focus_on_window(window->id);
                raise_window(window->id);
                return;
            }
        }

        element = element->next;

        if (!element) {
            element = window_list_head;
        }

    }

}
