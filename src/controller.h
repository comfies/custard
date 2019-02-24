#ifndef CONTROLLER_H
#define CONTROLLER_H

char buffer[1024];

unsigned short initialize_controller(void);
void finalize_controller(void);

void controller_input(int argc, char **argv);

#endif /* CONTROLLER_H */


