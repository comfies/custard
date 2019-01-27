#ifndef UTILITIES_H
#define UTILITIES_H

#include <pcre.h>

unsigned short regex_match(char *, char *);

unsigned short parse_boolean(const char *);
unsigned int parse_unsigned_integer(const char *);
unsigned int parse_rgba_color(const char *);

#endif /* UTILITIES_H */
