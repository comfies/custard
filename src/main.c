#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipc/controller.h"
#include "wm/custard.h"

int main(int argc, char** argv) {
    if (should_become_controller(argc, argv))
        return controller(argc, argv);
    
    return custard();
}
