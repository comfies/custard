#include <pcre.h>
#include <stdlib.h>
#include <string.h>

#include "custard.h"
#include "rules.h"
#include "vector.h"

void create_new_geometry(char *name, unsigned int x, unsigned int y,
    unsigned int height, unsigned int width) {

    named_geometry_t *geometry = NULL;
    for (unsigned int index = 0; index < named_geometries->size; index++) {
        geometry = get_from_vector(named_geometries, index);

        if (!strcmp(geometry->name, name)) {
            debug_output("Geometry already exists, changing properties");
            geometry->x = x;
            geometry->y = y;
            geometry->height = height;
            geometry->width = width;

            return;
        }
    }

    debug_output("Creating new named geometry");

    geometry = (named_geometry_t *)malloc(sizeof(named_geometry_t));
    geometry->name = (char *)malloc(sizeof(char));

    strcpy(geometry->name, name);
    geometry->x = x;
    geometry->y = y;
    geometry->height = height;
    geometry->width = width;

    push_to_vector(named_geometries, geometry);
}

void create_new_rule(char **arguments) {

    char *expression = arguments[0];
    char *named_geometry = arguments[1];

    window_rule_t *rule = NULL;
    for (unsigned int index = 0; index < window_rules->size; index++) {
        rule = get_from_vector(window_rules, index);

        if (!strcmp(rule->expression, expression)) {
            debug_output("%s",
                "Rule for expression already exists, changing named geometry");

            strcpy(rule->named_geometry, named_geometry);

            return;
        }
    }

    rule = (window_rule_t *)malloc(sizeof(window_rule_t));
    rule->expression = (char *)malloc(sizeof(char));

    rule->named_geometry = (char *)malloc(sizeof(char));
    rule->workspace = VALUE_UNCHANGED;

    strcpy(rule->expression, expression);
    strcpy(rule->named_geometry, named_geometry);

    push_to_vector(window_rules, rule);
}

unsigned short regex_match(char *subject, char *expression) {
    pcre *compiled;
    pcre_extra *optimized;

    const char *error;
    int offset;

    compiled = pcre_compile(expression, PCRE_UTF8, &error, &offset, NULL);

    if (!compiled)
        return 0;

    optimized = pcre_study(compiled, 0, &error);

    int return_value = pcre_exec(compiled, optimized, subject, strlen(subject),
        0, 0, NULL, 0);

    pcre_free(compiled);

    if (optimized)
        pcre_free_study(optimized);

    if (return_value < 0)
        return 0;

    return 1;
}
