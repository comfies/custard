#ifndef IPC_H
#define IPC_H

#define NUMCOMMANDS 9

struct ipc_command {
    char *target;
    char *action;
    short unsigned (*routine)(char **);
};

extern struct ipc_command ipc_commands[][NUMCOMMANDS];

void process_command(char*);

short unsigned ipc_custard_halt(char **);
short unsigned ipc_custard_configure(char **);
short unsigned ipc_custard_new_geometry(char **);
short unsigned ipc_custard_new_rule(char **);

short unsigned ipc_window_close(char **);
short unsigned ipc_window_raise(char **);
short unsigned ipc_window_lower(char **);
short unsigned ipc_window_change_geometry(char **);

short unsigned ipc_workspace_focus(char **);

#endif /* IPC_H */
