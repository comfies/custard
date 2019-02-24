#include <stdio.h>
#include <string.h>

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>

#include "custard.h"
#include "controller.h"
#include "socket.h"

int main(int argc, char **argv) {
    if (argc >= 2 && !strcmp("--", argv[1])) {
        debug_output("Starting controller");

        if (!initialize_controller())
            return EXIT_FAILURE;

        char **arguments = malloc(sizeof(char *) * (argc - 2));

        for (int index = 2; index < argc; index++) {
            arguments[index - 2] = malloc(strlen(argv[index]) + 1);
            strcpy(arguments[index - 2], argv[index]);
        }

        controller_input(argc - 2, arguments);

        finalize_controller();

        for (int index = 0; index < (argc - 2); index++)
            free(arguments[index]);
        free(arguments);

        return EXIT_SUCCESS;
    }

    debug_output("Starting window manager");

    return start_custard();
}
