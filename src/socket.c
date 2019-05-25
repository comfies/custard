#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "custard.h"
#include "ipc.h"
#include "socket.h"

int socket_file_descriptor;
socket_mode_t socket_mode;

unsigned short initialize_socket() {
    struct sockaddr_un address;

    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    
    char *display = getenv("DISPLAY");
    char *user = getenv("USER");

    char file_path[19 + strlen(user) + strlen(display)];
    sprintf(file_path, "/tmp/custard.%s_%s.sock", user, display);
    
    snprintf(address.sun_path, sizeof(address.sun_path), file_path);

    socket_file_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);

    if (!socket_file_descriptor) {
        debug_output("Unable to open socket");
        return 0;
    }

    if (socket_mode == WINDOW_MANAGER) {
        if (bind(socket_file_descriptor, (struct sockaddr *)&address,
            sizeof(address)) < 0) {
            debug_output("Unable to bind socket");
            return 0;
        }

        if (listen(socket_file_descriptor, 1) < 0) {
            debug_output("Unable to listen to socket");
            return 0;
        }
    } else {
        if (connect(socket_file_descriptor, (struct sockaddr*)&address,
            sizeof(address)) < 0) {
            debug_output("Unable to connect to socket");
            return 0;
        }
    }

    return 1;
}

void finalize_socket() {
    debug_output("Called");

    if (socket_mode == WINDOW_MANAGER)
        unlink("/tmp/custard.sock");
    else
        close(socket_file_descriptor);
}

void write_to_socket(char *data) {
    debug_output("Outputting data to socket\n\t%s", data);

    write(socket_file_descriptor, data, strlen(data));

    return;
}

void read_from_socket() {
    int command_file_descriptor = accept(socket_file_descriptor, NULL, 0);

    if (!command_file_descriptor) {
        debug_output("Unable to accept connection to socket");
        return;
    }

    char *data = calloc(1025, sizeof(char));
    ssize_t data_length = read(command_file_descriptor, data, 1024);

    if (data_length) {
        data[data_length] = '\0';
        process_input(data);
    }

    free(data);
}
