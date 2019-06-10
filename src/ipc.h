#ifndef IPC_H
#define IPC_H

#define suppress_unused(parameter) (void)(parameter)
#define number_of_ipc_commands 9

struct ipc_command {
    char *command;
    unsigned short (*routine)(char **);
};

extern struct ipc_command ipc_commands[][number_of_ipc_commands];

void process_input(char*);

unsigned short parse_boolean(char *);
unsigned int parse_unsigned_integer(char *);
unsigned int parse_rgba_color(char *);

unsigned short ipc_command_wm_halt(char **);
unsigned short ipc_command_wm_configure(char **);
unsigned short ipc_command_wm_reconfigure(char **);

unsigned short ipc_command_new_geometry(char **);
unsigned short ipc_command_new_window_rule(char **);

unsigned short ipc_command_window_close(char **);
unsigned short ipc_command_window_raise(char **);
unsigned short ipc_command_window_lower(char **);
unsigned short ipc_command_window_change_geometry(char **);

unsigned short ipc_command_change_workspace(char **);

#endif /* IPC_H */
