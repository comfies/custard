#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <pthread.h>

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>

#include "custard.h"

#include "ipc.h"
#include "config.h"
#include "xcb.h"
#include "ewmh.h"
#include "socket.h"
#include "handlers.h"
#include "grid.h"
#include "window.h"
#include "group.h"

int
main(int argc, char** argv)
{

    int commandline_argument;

    while ((commandline_argument = getopt(argc, argv, "hd")) != -1) {
        switch (commandline_argument) {
            case 'h':
                printf("%s\n", "custard 2.0");
                exit(EXIT_SUCCESS);
                break;
            case 'd':
                debug = 1;
                debug_output("Debug log enabled via command-line.");
                break;
        }
    }

    return start_custard();

}

