#include "utilities.h"

#include <pcre.h>
#include <string.h>

unsigned short
regex_match(char *subject, char *expression)
{
    pcre *compiled;
    pcre_extra *optimized;

    const char *error;
    int offset;

    compiled = pcre_compile(expression, PCRE_UTF8, &error, &offset, NULL);

    if (compiled == NULL)
        return 0;

    optimized = pcre_study(compiled, 0, &error);

    int return_value = pcre_exec(compiled, optimized, subject, strlen(subject),
        0, 0, NULL, 0);

    pcre_free(compiled);

    if (optimized != NULL)
        pcre_free_study(optimized);

    if (return_value < 0)
        return 0;

    return 1;
}

unsigned short
parse_boolean(const char *input)
{
    if (input)
    {
        if (strcmp(input, "True") == 0 || strcmp(input, "true") == 0)
            return 1;
        else if (strcmp(input, "False") == 0 || strcmp(input, "false") == 0)
            return 0;
    }

    return 0;
}

unsigned int
parse_unsigned_integer(const char *input)
{
    if (input)
        return atoi(input);

    return 0;
}

unsigned int
parse_rgba_color(const char *input)
{
    unsigned int rgba = 0x000000ff;

    if (input)
    {
        char groups[9] = {
            input[1], /* R */
            input[2], /* R */
            input[3], /* G */
            input[4], /* G */
            input[5], /* B */
            input[6], /* B */
            input[7], /* A */
            input[8], /* A */
            '\0'
        };

        rgba = strtol(groups, NULL, 16);
    }

    unsigned int alpha = (rgba & 0x000000ff);
    unsigned int red = (rgba & 0xff000000) / 0x1000000;
    unsigned int green = (rgba & 0x00ff0000) / 0x10000;
    unsigned int blue = (rgba & 0x0000ff00) / 0x100;

    red = (red * alpha) / 255;
    green = (green * alpha) / 255;
    blue = (blue * alpha) / 255;

    unsigned int value = 0;

    value = (alpha * 0x1000000);
    value |= (red * 0x10000);
    value |= (green * 0x100);
    value |= blue;

    return value;
}
