#include <stdio.h>
#include <stdlib.h>

#include <xcb/xcb.h>

#include "custard.h"
#include "ewmh.h"
#include "vector.h"
#include "window.h"
#include "workspaces.h"

void create_new_workspace() {
    debug_output("Creating new workspace");

    workspace_t *workspace = (workspace_t *)malloc(sizeof(workspace_t));

    workspace->focused_window = XCB_WINDOW_NONE;

    push_to_vector(workspaces, workspace);
}

void focus_on_workspace(unsigned int workspace_number) {
    debug_output("Called");

    if (focused_workspace == workspace_number)
        return;

    workspace_t *workspace = get_from_vector(workspaces, workspace_number - 1);

    if (!workspace)
        return;

    debug_output("Workspace check passed");

    focused_window = XCB_WINDOW_NONE;

    window_t *window = NULL;
    for (unsigned int index = 0; index < managed_windows->size; index++) {
        window = get_from_vector(managed_windows, index);
        if (window->workspace == workspace_number)
            map_window(window->id);
        else
            unmap_window(window->id);
    }

    debug_output("Window map changes complete");

    focused_workspace = workspace_number;
    xcb_ewmh_set_current_desktop(ewmh_connection, 0, workspace_number);

    if (workspace->focused_window != XCB_WINDOW_NONE) {
        debug_output("Focusing on window from workspace");

        focus_on_window(workspace->focused_window);
        raise_window(workspace->focused_window);
    }

}

