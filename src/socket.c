#include "socket.h"

#include "custard.h"
#include "ipc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/socket.h>

int socket_file_descriptor;

unsigned short int
initialize_socket()
{
    struct sockaddr_un address;

    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, sizeof(address.sun_path), TEMP_PATH);
    unlink(TEMP_PATH);

    socket_file_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);

    if (socket_file_descriptor == -1) {
        debug_output("initialize_socket(): Invalid file descriptor, unable to create socket");
        return 0;
    }

    if (bind(socket_file_descriptor,
        (struct sockaddr *)&address, sizeof(address)) == -1) {
        debug_output("initialize_socket(): Unable to bind socket");
        return 0;
    }

    if (listen(socket_file_descriptor, 1) == -1) {
        debug_output("initialize_socket(): Unable to listen to socket");
        return 0;
    }

    return 1;
}

void
read_socket()
{
    int command_file_descriptor = accept(socket_file_descriptor, NULL, 0);

    if (command_file_descriptor == -1) {
        debug_output("read_socket(): Unable to accept connection");
        return;
    }

    debug_output("read_socket(): reading from socket");

    char *data = calloc(1025, sizeof(char));

    ssize_t data_length = read(command_file_descriptor, data, 1024);

    if (data_length > 0) {
        data[data_length] = '\0';
        process_command(data);
    }

    free(data);

}

void *
start_socket_read_loop(void *pointer)
{
    (void)pointer;

    debug_output("start_socket_read_loop(): started socket read loop");
    while (wm_running) {
        read_socket();
    }

    return NULL;
}

void
finalize_socket()
{
    debug_output("finalize_socket(): called");
    unlink(TEMP_PATH);
}

