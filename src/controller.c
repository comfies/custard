#include <stdio.h>
#include <string.h>

#include "custard.h"
#include "controller.h"
#include "socket.h"

char buffer[1024];

unsigned short initialize_controller() {
    socket_mode = CONTROLLER;

    if (!initialize_socket())
        return 0;

    return 1;
}

void finalize_controller() {
    if (strlen(buffer))
        write_to_socket(buffer);

    finalize_socket();
}

void controller_input(int argc, char **argv) {
    if (!argc) {
        debug_output("Argv pass-through empty, not sending output to IPC");
        return;
    }

    debug_output("%s", argv[0]);

    char *descriptor = argv[0];

    if (!strcmp("stop", descriptor) || !strcmp("halt", descriptor)) {
        sprintf(buffer, "halt");
        return;
    }

    if (strlen(descriptor) >= 1024) {
        debug_output("Buffer too large to output");
        return;
    }

    sprintf(buffer, descriptor);

    debug_output("%s", buffer);

    for (int index = 1; index < argc; index++) {
        if (strlen(buffer) + strlen(argv[index]) >= 1024) {
            debug_output("Buffer too large to output");
            return;
        }

        sprintf(buffer, "%s%c%s", buffer, 29, argv[index]);
    }
}
