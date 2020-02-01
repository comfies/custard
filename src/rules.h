#ifndef RULES_H
#define RULES_H

#define VALUE_UNCHANGED 0

#include "configuration.h"
#include "vector.h"

typedef struct {
    char *name;
    unsigned int x;
    unsigned int y;
    unsigned int height;
    unsigned int width;
} named_geometry_t;

typedef enum window_property {
    window_name = 0,
    window_class = 1
} window_property_t;

typedef struct {
    char *expression;
    window_property_t property;
    vector_t *ruleset;
} window_rule_t;

void create_new_geometry(char *, unsigned int, unsigned int, unsigned int,
    unsigned int);

void create_new_rule(char **);

unsigned short regex_match(char *, char *);

#endif /* RULES_H */
