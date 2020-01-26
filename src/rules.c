#include <pcre.h>

#include <stdlib.h>
#include <string.h>

#include "custard.h"
#include "ipc.h"
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
    char *property = arguments[0];
    char *expression = arguments[1];
    window_property_t window_property;

    if (!strcmp(property, "window.name"))
        window_property = window_name;
    else if (!strcmp(property, "window.class"))
        window_property = window_class;

    char *named_geometry = VALUE_UNCHANGED;
    unsigned int workspace = VALUE_UNCHANGED;
    char *screen = VALUE_UNCHANGED;

    unsigned int index = 2;
    char *argument, *argument_pointer;
    char *name;

    debug_output("New rule created using attribute %s",
        property);

    while (arguments[index]) {
        if (!strcmp(arguments[index], "\3"))
            break;

        argument_pointer = strdup(arguments[index]);
        argument = argument_pointer;
        name = strsep(&argument, "=");

        if (!strcmp("workspace", name))
            workspace = parse_unsigned_integer(argument);
        else if (!strcmp("geometry", name)) {
            named_geometry = (char *)malloc(sizeof(char));
            strcpy(named_geometry, argument);
        } else if (!strcmp("monitor", name)) {
            screen = (char *)malloc(sizeof(char));
            strcpy(screen, argument);
        }

        if (argument_pointer)
            free(argument_pointer);

        index++;
    }

    debug_output("Properties for rule gathered, testing for existing pattern");

    unsigned short modify_rule = 0;
    window_rule_t *rule = NULL;
    for (unsigned int index = 0; index < window_rules->size; index++) {
        rule = get_from_vector(window_rules, index);

        if (!strcmp(rule->expression, expression)) {
            debug_output("%s",
                "Rule for expression already exists, changing rule");
            modify_rule = 1;

            break;
        }
    }


    if (!modify_rule) {
        rule = (window_rule_t *)malloc(sizeof(window_rule_t));

        rule->expression = (char *)malloc(sizeof(char));
        rule->property = window_property;
        strcpy(rule->expression, expression);

        rule->workspace = rule->named_geometry = rule->screen = NULL;

        push_to_vector(window_rules, rule);
    }

    if (workspace != VALUE_UNCHANGED)
        rule->workspace = workspace;

    if (named_geometry != VALUE_UNCHANGED) {
        if (rule->named_geometry)
            free(rule->named_geometry);
        rule->named_geometry = named_geometry;
    }

    if (screen != VALUE_UNCHANGED) {
        if (rule->screen)
            free(rule->screen);
        rule->screen = screen;
    }

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
