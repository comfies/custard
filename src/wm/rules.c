#include <pcre.h>
#include <string.h>

#include "rules.h"

vector_t *rules = NULL;

rule_t *create_or_get_rule(window_attribute_t attribute, char *expression) {
    rule_t *rule;

    if (rules) {
        while ((rule = vector_iterator(rules))) {
            if (!strcmp(rule->expression, expression) &&
                rule->attribute == attribute) {
                reset_vector_iterator(rules);
                return rule;
            }
        }
    }

    rule = (rule_t*)malloc(sizeof(rule_t));

    rule->expression = (char*)malloc(sizeof(char));
    strcpy(rule->expression, expression);

    rule->attribute = attribute;
    rule->rules = NULL;

    return rule;
}

void add_rule(rule_t *rule) {
    if (rules) {
        rule_t *existing_rule;
        while ((existing_rule = vector_iterator(rules)))
            if (rule == existing_rule)
                return;
    } else rules = construct_vector();

    push_to_vector(rules, rule);
}

unsigned short expression_matches(char *expression, char *subject) {
    pcre *compiled;
    pcre_extra *optimized;

    const char *error;
    int offset;

    if (!(compiled = pcre_compile(expression, PCRE_UTF8, &error, &offset,
        NULL)))
        return 0;

    optimized = pcre_study(compiled, 0, &error);

    int return_value = pcre_exec(compiled, optimized, subject, strlen(subject),
        0, 0, NULL, 0);

    if (optimized) pcre_free_study(optimized);
    pcre_free(compiled);

    if (return_value < 0)
        return 0;

    return 1;
}
