#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "custard.h"

char *rc_path;
vector_t *configuration = NULL;

void setup_global_configuration() {
    configuration = construct_vector();

    kv_pair_t *setting;
    setting = create_or_get_kv_pair(configuration, "grid.rows");
    setting->value->number = 2;

    setting = create_or_get_kv_pair(configuration, "grid.columns");
    setting->value->number = 3;

    setting = create_or_get_kv_pair(configuration, "grid.margins");
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "grid.margin.top");
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "grid.margin.bottom");
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "grid.margin.left");
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "grid.margin.right");
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "borders");
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "border.size.inner");
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "border.size.outer");
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "border.color.focused");
    setting->value->number = 0xFFFFFFFF;

    setting = create_or_get_kv_pair(configuration, "border.color.unfocused");
    setting->value->number = 0xFF676767;

    setting = create_or_get_kv_pair(configuration, "border.color.background");
    setting->value->number = 0xFF000000;

    setting = create_or_get_kv_pair(configuration, "border.colors.flipped");
    setting->value->boolean = 0;

    setting = create_or_get_kv_pair(configuration, "workspaces");
    setting->value->number = 1;
}

kv_pair_t *create_or_get_kv_pair(vector_t *configuration, char *key) {
    kv_pair_t *kv_pair;

    while ((kv_pair = vector_iterator(configuration))) {
        if (!strcmp(kv_pair->key, key)) {
            reset_vector_iterator(configuration);
            return kv_pair;
        }
    }

    kv_pair = (kv_pair_t*)calloc(1, sizeof(kv_pair_t));

    kv_pair->key = (char*)calloc(strlen(key) + 1, sizeof(char));
    strcpy(kv_pair->key, key);

    kv_pair->value = (kv_value_t*)calloc(1, sizeof(kv_value_t));
    log_debug("New key-value pair created with key(%s)", key);

    push_to_vector(configuration, kv_pair);
    return kv_pair;
}

kv_value_t *get_value_from_key(vector_t *configuration, char *key) {
    kv_pair_t *kv_pair;

    while ((kv_pair = vector_iterator(configuration))) {
        if (!strcmp(kv_pair->key, key)) {
            reset_vector_iterator(configuration);
            return kv_pair->value;
        }
    }

    return NULL;
}

kv_value_t *get_value_from_key_with_fallback(vector_t *passed_configuration,
    char *key) {

    if (!passed_configuration)
        passed_configuration = configuration;

    kv_value_t *value;
    value = get_value_from_key(passed_configuration, key);

    if (passed_configuration != configuration)
        if (!value)
            value = get_value_from_key(configuration, key);

    return value;
}
