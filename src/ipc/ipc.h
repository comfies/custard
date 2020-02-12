#ifndef IPC_H
#define IPC_H

#include "../vector.h"
#include "../wm/config.h"

void ipc_process_input(char*);

void ipc_helper_typecast_and_assign(kv_value_t*, char*, char*);

void ipc_command_configure(vector_t*, unsigned short*);
void ipc_command_geometry(vector_t*, unsigned short*);
void ipc_command_match(vector_t*, unsigned short*);
void ipc_command_window(vector_t*, unsigned short*);

#endif
