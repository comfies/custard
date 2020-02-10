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

color_t string_to_color(char* string) {
    color_t color = {
        .red   = 255,
        .green = 255,
        .blue  = 255,
        .alpha = 255
    };

    if (!string || string[0] != '#')
        return color;

    unsigned int length = strlen(string);
    if (length <= 4 || (5 < length && length < 7) || length == 8 || length > 9)
        return color;

    unsigned int offset = 0;
    if (length < 6) offset = 4;

    char group[8] = "000000ff";
    for (unsigned int index = 0; index < (length - 1); index++)
        group[index + offset] = string[index + 1];

    unsigned int rgba;
    rgba = strtol(group, NULL, 16);

    if (offset) {
        color.alpha = (rgba & 0x000F) * 0x11;
        color.red   = ((rgba & 0xF000) / 0x1000) * 0x11;
        color.green = ((rgba & 0x0F00) / 0x100) * 0x11;
        color.blue  = ((rgba & 0x00F0) / 0x10) * 0x11;
    } else {
        color.alpha = (rgba & 0x000000FF);
        color.red   = (rgba & 0xFF000000) / 0x1000000;
        color.green = (rgba & 0x00FF0000) / 0x10000;
        color.blue  = (rgba & 0x0000FF00) / 0x100;
    }

    return color;
/*
    red   = (red * alpha) / 0xFF;
    green = (green * alpha) / 0xFF;
    blue  = (blue * alpha) / 0xFF;

    alpha *= 0x1000000;
    red   *= 0x10000;
    green *= 0x100;

    return alpha | red | green | blue;*/
}
