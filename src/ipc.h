#ifndef IPC_H
#define IPC_H

#define suppress_unused(parameter) (void)(parameter)

void process_input(char*);

unsigned short parse_boolean(char *);
unsigned int parse_unsigned_integer(char *);
unsigned int parse_rgba_color(char *);

void ipc_command_wm_halt(char **, unsigned short *);
void ipc_command_wm_configure(char **, unsigned short *);
void ipc_command_wm_reconfigure(char **, unsigned short *);

void ipc_command_new_geometry(char **, unsigned short *);
void ipc_command_new_window_rule(char **, unsigned short *);

void ipc_command_window_close(char **, unsigned short *);
void ipc_command_window_raise(char **, unsigned short *);
void ipc_command_window_lower(char **, unsigned short *);

void ipc_command_window_manual_resize(char **, unsigned short *);
void ipc_command_window_manual_move(char **, unsigned short *);
void ipc_command_window_change_geometry(char **, unsigned short *);

void ipc_command_change_workspace(char **, unsigned short *);

#endif /* IPC_H */
