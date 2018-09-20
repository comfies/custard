#include "ipc.h"

#include "config.h"
#include "grid.h"
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

    token = strtok(input, ";");

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
            token = strtok(NULL, ";");
        }
    }

    if (!target || !action) {
        return;
    }

    debug_output("Input split");

    if (target == 0xEAC4D763) {
        /* target: custard */

        switch (action) {
            case 0x7C79369D: /* halt */
                wm_running = 0;
                break;

            case 0x3D2C748D: /* configure */

                if (strcmp("grid_rows", arguments[0]) == 0) {
                    grid_rows = parse_unsigned_integer(arguments[1]);
                } else if (strcmp("grid_columns", arguments[0]) == 0) {
                    grid_columns = parse_unsigned_integer(arguments[1]);
                } else if (strcmp("grid_gap", arguments[0]) == 0) {
                    grid_gap = parse_unsigned_integer(arguments[1]);
                }

                grid_apply_configuration();

                return; /* Don't unnecessarily commit to X */
                break;

            case 0xEC16043C: /* new_geometry */

                new_geometry(arguments[0],
                    parse_unsigned_integer(arguments[1]),
                    parse_unsigned_integer(arguments[2]),
                    parse_unsigned_integer(arguments[3]),
                    parse_unsigned_integer(arguments[4]));

                return;
                break;

            case 0xCD252BAD: /* new_geometry_rule */
                new_geometry_rule(
                    (window_attribute_t)parse_unsigned_integer(arguments[0]),
                    arguments[1],
                    arguments[2]);

                return;
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
                struct LinkedListElement *element = geometry_list_head;

                if (!element) {
                    return;
                }

                while (element) {
                    debug_output("Testing geometry %s to %s.",
                        ((Geometry *)element->data)->name, arguments[0]);
                    if (strcmp(((Geometry *)element->data)->name,
                        arguments[0]) == 0) {

                        move_window_to_grid_coordinate(window_id,
                            ((Geometry *)element->data)->x,
                            ((Geometry *)element->data)->y);
                        resize_window_with_grid_units(window_id,
                            ((Geometry *)element->data)->height,
                            ((Geometry *)element->data)->width);

                        commit();
                        return;
                    }

                    element = element->next;
                }
/*                    struct NamedGeometry geometry;

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
                    }*/
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
