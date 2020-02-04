#ifndef RULES_H
#define RULES_H

#include "../vector.h"

extern vector_t* rules;

typedef enum window_attribute {
    name = 0,
    class = 1,
} window_attribute_t;

typedef struct {
    char* expression;
    window_attribute_t attribute;
    vector_t* rules;
} rule_t;

rule_t* create_or_get_rule(window_attribute_t, char*);
void add_rule(rule_t*);
unsigned short expression_matches(char*, char*);

#endif
