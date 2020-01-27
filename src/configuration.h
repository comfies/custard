#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "vector.h"

typedef struct {
    char *name;
    void *value;
} setting_t;

setting_t *create_setting(char *name, void *);
void *query_setting(vector_t *, char *name);
unsigned short append_setting(vector_t *, char *, void *);
unsigned short update_setting(vector_t *, char *, void *);

#endif /* CONFIGURATION_H */
