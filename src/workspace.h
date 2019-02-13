#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "window.h"


struct Workspace {
    xcb_window_t focused_window;
    unsigned int id;
};

extern struct Workspace *focused_workspace;
extern struct LinkedListElement *workspace_list_head;

void initialize_workspaces(void);
void finalize_workspaces(void);

struct Workspace *get_workspace(unsigned int);
void focus_on_workspace(unsigned int);

/*
typedef enum {
    UNFOCUSED,
    FOCUSED
} workspace_state_t;

extern short unsigned int workspaces;
extern unsigned int focused_workspace;

workspace_state_t get_workspace_state(unsigned int);

short unsigned int window_is_in_workspace(Window *, unsigned int);

void attach_window_to_workspace(xcb_window_t, unsigned int);

void focus_on_workspace(unsigned int);
*/
#endif /* WORKSPACE_H */
