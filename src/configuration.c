#include <string.h>
#include <stdlib.h>

#include "custard.h"
#include "configuration.h"
#include "vector.h"

setting_t *create_setting(char *name, void *value) {
    setting_t *setting = (setting_t *)malloc(sizeof(setting_t));
    setting->name = (char *)malloc(sizeof(char));

    strcpy(setting->name, name);
    setting->value = value;

    return setting;
}

void *query_setting(vector_t *configuration, char *name) {
    setting_t *setting;
    unsigned int index = 0;
    for (; configuration->size; index++) {
        setting = get_from_vector(configuration, index);

        if (strcmp(setting->name, name))
            continue;

        return setting->value;
    }

    debug_output("Queried setting(%s) returned no value, %s",
        "expect a possible segfault", name);

    return NULL;
}

unsigned short append_setting(vector_t *configuration, char *name,
    void *value) {
    setting_t *setting = create_setting(name, value);
    push_to_vector(configuration, setting);

    return 1;
}

unsigned short update_setting(vector_t *configuration, char *name,
    void *value) {
    setting_t *setting;
    unsigned int index = 0;
    for (; configuration->size; index++) {
        setting = get_from_vector(configuration, index);

        if (strcmp(setting->name, name))
            continue;

        setting->value = value;
        return 1;
    }

    return 0;
}
