
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_xrm.h>
#include <xcb/xcb_aux.h>

#include <iostream>
#include <map>
#include <vector>
#include <regex>

#include "xcb.hpp"
#include "ewmh.hpp"
#include "window.hpp"
#include "workspaces.hpp"
#include "fifo.hpp"
#include "custard.hpp"

#include "handlers.cpp"
#include "xcb.cpp"
#include "ewmh.cpp"
#include "config.cpp"
#include "grid.cpp"
#include "window.cpp"
#include "workspaces.cpp"
#include "commands.cpp"
#include "fifo.cpp"
#include "custard.cpp"

int main(int argc, char* argv[]) {

    atexit(custard::stop);
    custard::start();

    return 0;

}

