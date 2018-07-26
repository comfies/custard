#ifndef SOCKET_H
#define SOCKET_H

extern int socket_file_descriptor;

unsigned short int initialize_socket(void);
void read_socket(void);
void *start_socket_read_loop(void *);
void finalize_socket(void);

#endif /* SOCKET_H */
