#include "workspace.h"

#include "config.h"
#include "window.h"
#include "ewmh.h"
#include "utilities.h"

struct Workspace *focused_workspace = NULL;
struct LinkedListElement *workspace_list_head = NULL;

void
initialize_workspaces() {
    debug_output("Initializing workspaces.");

    // Initialize first workspace

    struct Workspace *workspace;
    workspace = (struct Workspace *)malloc(sizeof(struct Workspace));

    workspace->id = 1;
    workspace->focused_window = XCB_WINDOW_NONE;

    workspace_list_head = (struct LinkedListElement *)malloc(
        sizeof(struct LinkedListElement));

    workspace_list_head->data = workspace;
    workspace_list_head->next = NULL;

    focused_workspace = workspace;
    debug_output("Initialized first focused workspace");

    struct LinkedListElement *element = workspace_list_head;

    while (element->next)
        element = element->next;

    for (unsigned int index = 1; index < num_workspaces; index++) {
        workspace = (struct Workspace *)malloc(sizeof(struct Workspace));
        workspace->id = index + 1;
        workspace->focused_window = XCB_WINDOW_NONE;

        debug_output("Created non-first workspace");

        struct LinkedListElement *next_element;
        next_element = (struct LinkedListElement *)malloc(
            sizeof(struct LinkedListElement));

        debug_output("Created next linked list element");

        next_element->next = NULL;
        next_element->data = workspace;

        debug_output("Set element data");

        element->next = next_element;
        element = element->next;
        debug_output("Set next element in list");
    }
}

void
finalize_workspaces() {
    struct LinkedListElement *element = workspace_list_head;
    struct LinkedListElement *old_element = NULL;

    while (element) {
        old_element = element;
        element = element->next;

        free(old_element->data);
        free(old_element);
    }
}

struct Workspace *
get_workspace(unsigned int workspace) {
    if (workspace < 1 || workspace > num_workspaces)
        return NULL;

    struct LinkedListElement *element = workspace_list_head;

    if (!element)
        return NULL;

    debug_output("List head");

    while (element) {
        if (workspace == ((struct Workspace *)element->data)->id)
            return (struct Workspace *)element->data;
        debug_output("Continuing, no match");

        element = element->next;
    }

    return NULL;
}

void
focus_on_workspace(unsigned int workspace_num) {
    struct Workspace *workspace = get_workspace(workspace_num);

    if (!workspace)
        return;

    if (workspace == focused_workspace)
        return;

    debug_output("Focusing on workspace %d", workspace_num);

    struct LinkedListElement *element = window_list_head;
    Window *window = NULL;

    while (element) {
        window = element->data;

        if (window->workspace == workspace_num) {
            map_window(window->id);
        } else {
            unmap_window(window->id);
        }

        element = element->next;
    }

    focused_workspace = workspace;
    xcb_ewmh_set_current_desktop(ewmh_connection,
        0, workspace_num);

    if (focused_workspace->focused_window != XCB_WINDOW_NONE) {
        focus_on_window(focused_workspace->focused_window);
        raise_window(focused_workspace->focused_window);
    }

    return;

}


