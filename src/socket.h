#ifndef SOCKET_H
#define SOCKET_H

extern int socket_file_descriptor;

typedef enum {
    WINDOW_MANAGER,
    CONTROLLER
} socket_mode_t;

extern socket_mode_t socket_mode;

unsigned short initialize_socket(void);
void finalize_socket(void);

void write_to_socket(char *);
void read_from_socket(void);

#endif /* SOCKET_H */
