#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "socket.h"

#include "../wm/custard.h"

int socket_file_descriptor;
socket_mode_t socket_mode;
char *socket_path;

unsigned short initialize_socket() {
    struct sockaddr_un address;

    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;

    char *display = getenv("DISPLAY");
    char *user = getenv("USER");

    socket_path = (char*)calloc((20 + strlen(user) + strlen(display)),
        sizeof(char));
    sprintf(socket_path, "/tmp/custard.%s_%s.sock", user, display);
    snprintf(address.sun_path, sizeof(address.sun_path), "%s", socket_path);

    socket_file_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);

    if (!socket_file_descriptor) {
        log_fatal("Unable to open socket");
        return 0;
    }

    if (socket_mode == WINDOW_MANAGER) {
        if (bind(socket_file_descriptor, (struct sockaddr *)&address,
            sizeof(address)) < 0) {
            log_fatal("Unable to bind socket");
            return 0;
        }

        if (listen(socket_file_descriptor, 1) < 0) {
            log_fatal("Unable to listen to socket");
            return 0;
        }
    } else if (socket_mode == CONTROLLER) {
        if (connect(socket_file_descriptor, (struct sockaddr*)&address,
            sizeof(address)) < 0) {
            log_fatal("Unable to connect to socket");
            return 0;
        }
    }

    return 1;
}

void finalize_socket() {
    if (socket_mode == WINDOW_MANAGER)
        unlink(socket_path);
    else
        close(socket_file_descriptor);
    free(socket_path);
}

void write_to_socket(char *input) {
    log_debug("Feeding data to socket(%s)", input);

    write(socket_file_descriptor, input, strlen(input));
}

char *read_from_socket() {
    int command_file_descriptor = accept(socket_file_descriptor, NULL, 0);

    if (!command_file_descriptor) {
        log_fatal("Unable to accept connection to socket");
        return "";
    }

    char *output = calloc(1024, sizeof(char));
    ssize_t length = read(command_file_descriptor, output, 1023);

    output[length] = '\0';
    log_debug("Fed data(%s)", output);

    return output;
}
