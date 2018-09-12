#include "ipc.h"

#include "config.h"
#include "group.h"
#include "xcb.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
process_command(char *input)
{
    debug_output("beginning of received input >>\n %s <<end of received input", input);

    unsigned int target = 0;
    unsigned int action = 0;
    char *arguments[8];

    char *token;
    unsigned int index = 0;

    token = strtok(input, " ");

    while (token) {
        if (index == 0) {
            target = parse_unsigned_integer(token);
        } else if (index == 1) {
            action = parse_unsigned_integer(token);
        } else {
            arguments[index - 2] = token;
        }

        token = NULL;
        if (++index < 10) {
            token = strtok(NULL, " ");
        }
    }

    if (!target || !action) {
        return;
    }

    debug_output("Input split");

    if (target == 0xEAC4D763) {
        /* target: custard */

        switch (action) {
            case 0x7C716C94: /* halt */
                wm_running = 0;
                break;

            default:
                return;
        }

    } else if (target == 0x782DC389) {
        /* target: window */

        if (!focused_window) {
            return;
        }

        xcb_window_t window_id = focused_window->id;

        switch (action) {
            case 0xA8B6733:
                close_window(window_id);
                break;

            case 0xB9A8149:
                raise_window(window_id);
                break;

            case 0xA53CBC6:
                lower_window(window_id);
                break;

            case 0x5769B7BF: {
                    struct NamedGeometry geometry;

                    for (unsigned int index = 0;
                        geometries[index].name; index++) {
                        geometry = geometries[index];

                        if (strcmp(geometry.name, arguments[0]) == 0) {
                            move_window_to_grid_coordinate(window_id,
                                geometry.geometry.x, geometry.geometry.y);
                            resize_window_with_grid_units(window_id,
                                geometry.geometry.height,
                                geometry.geometry.width);
                            commit();
                            return;
                        }
                    }
                }
                break;

            default:
                return;
        }

    }

    commit();
}

unsigned short int
parse_boolean(const char *input)
{
    if (input) {
        if (strcmp(input, "True") == 0 || strcmp(input, "true") == 0) {
            return 1;
        } else if (strcmp(input, "False") == 0 ||
            strcmp(input, "false") == 0) {
            return 0;
        }
    }

    return 0;
}

unsigned int
parse_unsigned_integer(const char *input)
{
    if (input) {
        return atoi(input);
    }

    return 0;
}

unsigned int
parse_rgba_color(const char *input)
{
    if (input) {
        unsigned int RGBA;

        char groups[9] = {
            input[7], /* A */
            input[8], /* A */
            input[1], /* R */
            input[2], /* R */
            input[3], /* G */
            input[4], /* G */
            input[5], /* B */
            input[6], /* B */
            '\0'
        };

        RGBA = strtol(groups, NULL, 16);

        return RGBA;
    }

    return 0x000000FF;
}
