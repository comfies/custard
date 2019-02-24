#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
    } else {
        rc_file = (char *)malloc(sizeof(char));

        // Process arguments
        char *argument;

        for (int index = 1; index < argc; index++) {
            argument = argv[index];

            if (!strcmp(argument, "--debug")) {
                debug_mode = 1;
                debug_output("Setting debug mode from command line arguments");
            } else if (!strcmp(argument, "--rc")) {
                if (!argv[index + 1]) {
                    debug_output("No rc specified with --rc, exiting");
                    return EXIT_FAILURE;
                }

                if (access(argv[index + 1], X_OK | F_OK | R_OK) > -1) {
                    strcpy(rc_file, argv[index + 1]);

                    index++;
                    continue;
                } else {
                    debug_output("rc specified is not executable, exiting");
                    return EXIT_FAILURE;
                }
            }

        }

    }

    debug_output("Starting window manager");

    return start_custard();
}
