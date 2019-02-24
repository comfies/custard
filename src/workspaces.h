#ifndef WORKSPACES_H
#define WORKSPACES_H

typedef struct {
    xcb_window_t focused_window;
} workspace_t;

void create_new_workspace(void);
void focus_on_workspace(unsigned int);

#endif /* WORKSPACES_H */
