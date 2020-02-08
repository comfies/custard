#include <string.h>
#include <stdlib.h>

#include "config.h"

vector_t* configuration;

void setup_global_configuration() {
    configuration = construct_vector();

    kv_pair_t* setting;
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
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "border.color.unfocused");
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "border.color.background");
    setting->value->number = 0;

    setting = create_or_get_kv_pair(configuration, "border.colors.flipped");
    setting->value->boolean = 0;

    setting = create_or_get_kv_pair(configuration, "workspaces");
    setting->value->number = 1;
}

kv_pair_t* create_or_get_kv_pair(vector_t* configuration, char* key) {
    kv_pair_t* kv_pair;

    for (unsigned int index = 0; index < configuration->size; index++) {
        kv_pair = get_from_vector(configuration, index);
        if (!strcmp(kv_pair->key, key))
            return kv_pair;
    }

    kv_pair = (kv_pair_t*)malloc(sizeof(kv_pair_t));

    kv_pair->key = (char*)malloc(sizeof(char));
    strcpy(kv_pair->key, key);

    kv_pair->value = (kv_value_t*)malloc(sizeof(kv_value_t));

    push_to_vector(configuration, kv_pair);
    return kv_pair;
}

kv_value_t* get_value_from_key(vector_t* configuration, char* key) {
    kv_pair_t* kv_pair;
    for (unsigned int index = 0; index < configuration->size; index++) {
        kv_pair = get_from_vector(configuration, index);

        if (!strcmp(kv_pair->key, key)) {
            return kv_pair->value;
        }
    }

    return NULL;
}

kv_value_t* get_value_from_key_with_fallback(vector_t* passed_configuration,
    char* key) {

    if (!passed_configuration)
        passed_configuration = configuration;

    kv_value_t* value;
    value = get_value_from_key(passed_configuration, key);

    if (!value)
        value = get_value_from_key(configuration, key);

    return value;
}