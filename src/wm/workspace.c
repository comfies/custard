#include "custard.h"
#include "window.h"
#include "workspace.h"

#include "../xcb/window.h"

unsigned int current_workspace = 1;

void show_workspace_on_monitor(monitor_t *monitor, unsigned int workspace) {
    // TODO: test for integrity of workspace
    if (!workspace)
        return; // workspace 0 unavailable

    unsigned int workspaces = get_value_from_key_with_fallback(
        monitor->configuration, "workspaces")->number;

    if (workspaces < workspace)
        return;

    monitor->workspace = workspace;

    window_t *window = NULL;
    while ((window = vector_iterator(windows))) {
        if (window->monitor != monitor)
            continue;

        if (monitor->workspace == window->workspace)
            map_window(window->parent);
        else
            unmap_window(window->parent);
    }
}
