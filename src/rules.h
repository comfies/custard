#ifndef RULES_H
#define RULES_H

typedef struct {
    char *name;
    unsigned int x;
    unsigned int y;
    unsigned int height;
    unsigned int width;
} named_geometry_t;

typedef struct {
    char *expression;
    char *named_geometry;
} window_rule_t;

void create_new_geometry(char *, unsigned int, unsigned int, unsigned int,
    unsigned int);

void create_new_rule(char *, char *);

unsigned short regex_match(char *, char *);

#endif /* RULES_H */
