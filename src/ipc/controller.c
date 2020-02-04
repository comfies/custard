#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "controller.h"
#include "socket.h"

#include "../wm/custard.h"

unsigned short should_become_controller(int argc, char** argv) {
    if (argc >= 2 && !strcmp(argv[1], "-"))
        return 1;

    return 0;
}

int controller(int argc, char** argv) {
    log("Instance became controller");

    socket_mode = CONTROLLER;

    if (!initialize_socket())
        return EXIT_FAILURE;

    char* input_feed = malloc(sizeof(char) * 1023);

    for (int index = 2; index < argc; index++) {
        strcat(input_feed, argv[index]);
        input_feed[strlen(input_feed)] = '\31';
    }
    input_feed[strlen(input_feed) - 1] = '\0';

    write_to_socket(input_feed);
    free(input_feed);

    finalize_socket();

    return EXIT_SUCCESS;
}
