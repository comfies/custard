#include <stdlib.h>
#include <string.h>

#include "ipc.h"
#include "parsing.h"

#include "../vector.h"
#include "../wm/custard.h"
#include "../wm/rules.h"
#include "../xcb/connection.h"

void ipc_process_input(char* feed) {
    /* Really shitty IPC but who honestly cares */
    unsigned short update = 0;
    vector_t* input = construct_vector();

    char* token;
    while ((token = strsep(&feed, "\31")))
        push_to_vector(input, token);

    char* qualifier = get_from_vector(input, 0);

    // TODO: literally all of this

    if (!strcmp(qualifier, "if")) {
        if (input->size <= 2 || input->size % 2)
            return;

        char* subject_and_expression = get_from_vector(input, 1);
        token = strsep(&subject_and_expression, ":");
        char* expression = subject_and_expression;

        window_attribute_t attribute;
        if (!strcmp(token, "window.name")) {
            attribute = name;
        } else if (!strcmp(token, "window.class")) {
            attribute = class;
            // else monitor
        } else return;

        rule_t* rule = create_or_get_rule(attribute, expression);
        // TODO: add rules to the actual rule set lol
        add_rule(rule);

        log("Rule added");
    }

    if (update)
        apply();

    deconstruct_vector(input);
}
