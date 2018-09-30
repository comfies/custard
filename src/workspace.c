#include "workspace.h"

#include "config.h"
#include "window.h"
#include "ewmh.h"

short unsigned int workspaces = 0;
unsigned int focused_workspace = 1;

workspace_state_t
get_workspace_state(unsigned int workspace) {
    debug_output("Called");
    workspace_state_t state;

    if (workspace < 1 || workspace > 16 || workspace > num_workspaces) {
        return UNFOCUSED;
    }

    state = UNFOCUSED;
    if (focused_workspace == workspace) {
        state = FOCUSED;
    }

    return state;
}

short unsigned int
window_is_in_workspace(Window *window, unsigned int workspace) {
    if ((window->workspaces & (1 << (workspace - 1))) > 0) {
        return 1;
    }

    return 0;
}

void
attach_window_to_workspace(xcb_window_t window_id, unsigned int workspace) {
    debug_output("Called");

    if (workspace < 1 || workspace > 16 || workspace > num_workspaces) {
        return;
    }

    Window *window = window_list_get_window(window_id);

    if (!window) {
        return;
    }

    window->workspaces = (1 << (workspace - 1));

    if (window->workspaces & workspaces) {
        map_window(window->id);
    } else {
        unmap_window(window->id);
    }
}

void
focus_on_workspace(unsigned int workspace) {
    debug_output("Called");

    if (workspace < 1 || workspace > 16 || workspace > num_workspaces) {
        return;
    }

    if (focused_window) {
        if (!window_is_in_workspace(focused_window, workspace)) {
            unfocus_window();
        }
    }

    struct LinkedListElement *element = window_list_head;
    Window *window = NULL;
    xcb_window_t last_window = XCB_WINDOW_NONE;
    /* TODO: store last focused window on workspace so users
        switch back to workspaces without the window focus changing */

    while (element) {
        window = element->data;

        if (window_is_in_workspace(window, workspace)) {
            map_window(window->id);
            last_window = window->id;
        } else {
            unmap_window(window->id);
        }

        element = element->next;
    }

    workspaces = (1 << (workspace - 1));
    focused_workspace = workspace;

    xcb_ewmh_set_current_desktop(ewmh_connection, 0, focused_workspace);

    if (last_window != XCB_WINDOW_NONE) {
        focus_on_window(last_window);
        raise_window(last_window);
    }
}
