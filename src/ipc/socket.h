#ifndef SOCKET_H
#define SOCKET_H

typedef enum {
    WINDOW_MANAGER = 0,
    CONTROLLER = 1
} socket_mode_t;

extern int socket_file_descriptor;
extern socket_mode_t socket_mode;

unsigned short initialize_socket(void);
void finalize_socket(void);

void write_to_socket(char*);
char* read_from_socket(void);

#endif
