#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    void** elements;
    unsigned int memory;
    unsigned int size;
} vector_t;

vector_t* construct_vector(void);
void push_to_vector(vector_t*, void*);
void pull_from_vector(vector_t*, unsigned int);
void* get_from_vector(vector_t*, unsigned int);
void deconstruct_vector(vector_t*);

#endif
