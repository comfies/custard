#include <stdlib.h>
#include <string.h>

#include "parsing.h"

unsigned short string_to_boolean(char* string) {
    if (!strcmp(string, "True") || !strcmp(string, "true"))
        return 1;

    return 0;
}

unsigned int string_to_integer(char* string) {
    return atoi(string);
}

unsigned int string_to_color(char* string) {
    unsigned int rgba = 0xffffffff;

    if (!string)
        return rgba;

    if (string[0] != '#')
        return rgba;

    unsigned int length = strlen(string);
    if (length <= 4 || (5 < length && length < 7) || length == 8 || length > 9)
        return rgba;

    unsigned int offset = 0;
    if (length < 6) offset = 4;

    char group[8] = "000000ff";
    for (unsigned int index = 0; index < (length - 1); index++)
        group[index + offset] = string[index + 1];

    rgba = strtol(group, NULL, 16);

    unsigned int alpha, red, green, blue;
    if (offset) {
        alpha = (rgba & 0x000F) * 0x11;
        red   = ((rgba & 0xF000) / 0x1000) * 0x11;
        green = ((rgba & 0x0F00) / 0x100) * 0x11;
        blue  = ((rgba & 0x00F0) / 0x10) * 0x11;
    } else {
        alpha = (rgba & 0x000000FF);
        red   = (rgba & 0xFF000000) / 0x1000000;
        green = (rgba & 0x00FF0000) / 0x10000;
        blue  = (rgba & 0x0000FF00) / 0x100;
    }

    red   = (red * alpha) / 0xFF;
    green = (green * alpha) / 0xFF;
    blue  = (blue * alpha) / 0xFF;

    alpha *= 0x1000000;
    red   *= 0x10000;
    green *= 0x100;

    return alpha | red | green | blue;
}
