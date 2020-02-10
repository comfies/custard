#ifndef CONFIG_H
#define CONFIG_H

#include "../vector.h"

extern vector_t* configuration;

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} color_t;

typedef union kv_value {
    unsigned short boolean;
    unsigned int number;
    char* string;
    color_t color;
} kv_value_t;

typedef struct {
    char* key;
    kv_value_t* value;
} kv_pair_t;

void setup_global_configuration(void);
kv_pair_t* create_or_get_kv_pair(vector_t*, char*);
kv_value_t* get_value_from_key(vector_t*, char*);
kv_value_t* get_value_from_key_with_fallback(vector_t*, char*);

#endif
