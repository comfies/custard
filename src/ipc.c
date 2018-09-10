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
    debug_output("process_command(): beginning of received input");
    debug_output(input);
    debug_output("process_command(): end of received input");

    char *diced[4];
    char *token;
    unsigned int index = 0;

    token = strtok(input, " ");

    if (!token) {
        diced[0] = input;
    } else {
        while (token) {
            diced[index] = token;
            index++;
            if (index < 5) {
                token = strtok(NULL, " ");
            } else {
                token = NULL;
            }
        }
    }

    debug_output("process_command(): output split");

    if (!diced[0] || !diced[1]) {
        return;
    }

    unsigned long hash = 5381;
    unsigned int target = 0, action = 0;

    int c;

    while ((c = *diced[0]++)) {
        hash = (hash * 33) ^ c;
    }
    target = hash >> target;
    hash = 5381;

    while ((c = *diced[1]++)) {
        hash = (hash * 33) ^ c;
    }
    action = hash >> action;

    short unsigned boolean = 0;
    unsigned int n = 0;

    switch (target) {
        case 3938768739: /* custard */

            switch (action) {
                case 2087808148: /* halt */
                    wm_running = 0;
                    break;

                case 171175241: /* focus */
                    if (!diced[2]) {
                        return;
                    }

                    focus_next_window();
                    break;

                case 1026323597: /* configure */
                    if (!diced[2] || !diced[3]) {
                        return;
                    }

                    hash = 5381;
                    unsigned int setting = 0;

                    while ((c = *diced[2]++)) {
                        hash = (hash * 33) ^ c;
                    }
                    setting = hash >> setting;

                    boolean = parse_boolean(diced[3]);
                    n = parse_unsigned_integer(diced[3]);
                    unsigned int color = parse_rgba_color(diced[3]);

                    switch (setting) {
                        case 2523544616: /* debug_mode */
                            debug = boolean;
                            break;

                        case 232847129: /* border_focused_color */
//                            border_focused_color = color;
                            break;

                        case 3731375234: /* border_unfocused_color */
//                            border_unfocused_color = color;
                            break;

                        case 1539877306: /* border_background_color */
//                            border_background_color = color;
                            break;

                        case 670643221: /* border_invert_colors */
//                            border_invert_colors = boolean;
                            break;

                        case 374560946: /* border_inner_size */
//                            border_inner_size = n;
                            break;

                        case 1738679797: /* border_outer_size */
//                            border_outer_size = n;
                            break;

                        case 368571342: /* border_type */
                            if (n > 3) {
                                n = 3;
                            }

//                            border_type = n;
                            break;

                        case 3436447195: /* grid_rows */
//                            grid_rows = n;
                            break;

                        case 2093021703: /* grid_columns */
//                            grid_columns = n;
                            break;

                        case 2316679092: /* grid_gap */
//                            grid_gap = n;
                            break;

                        case 2396801352: /* grid_margin_top */
//                            grid_margin_top = n;
                            break;

                        case 1882181388: /* grid_margin_bottom */
//                            grid_margin_bottom = n;
                            break;

                        case 1784735736: /* grid_margin_left */
//                            grid_margin_left = n;
                            break;

                        case 3069105795: /* grid_margin_right */
//                            grid_margin_right = n;
                            break;

                        case 1394395017: /* groups */
//                            groups = n;
                            break;

                        default:
                            return;
                    }

/*                    apply_config();*/
                    break;

                default:
                    return;
            }

            break;

        case 2016265097: /* window */
            if (!focused_window || !diced[2]) {
                return;
            }

            xcb_window_t window_id = focused_window->id;

            n = parse_unsigned_integer(diced[2]);

            cardinal_direction_t direction;
            direction = (cardinal_direction_t)n;

            switch (action) {
                case 176908083: /* close */
                    close_window(window_id);
                    break;

                case 194675017: /* raise */
                    raise_window(window_id);
                    break;

                case 173263814: /* lower */
                    lower_window(window_id);
                    break;

                case 2087699060: /* move */
                    move_window_cardinal(window_id, direction);
                    break;

                case 1179764707: /* expand */
                    expand_window_cardinal(window_id, direction);
                    break;

                case 2398914583: /* contract */
                    contract_window_cardinal(window_id, direction);
                    break;

                case 1459515434: /* attach_to_group */
                    attach_window_to_group(window_id, n);
                    break;

                case 1947222643: /* detach_from_group */
                    detach_window_from_group(window_id, n);
                    break;

                case 2605769987: /* use_geometry */ {
                        struct NamedGeometry geometry;

                        for (unsigned int index = 0; geometries[index].name; index++) {
                            geometry = geometries[index];

                            if (strcmp(geometry.name, diced[2]) == 0) {
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

            break;

        case 172404922: /* group */
            if (!diced[2]) {
                return;
            }

            n = parse_unsigned_integer(diced[2]);

            switch (action) {
                case 171175241: /* focus */
                    focus_group(n);
                    break;

                case 1322218894: /* attach */
                    map_group(n);
                    break;

                case 1121335546: /* detach */
                    unmap_group(n);
                    break;

                default:
                    return;
            }

            break;

        default:
            return;
    }

    commit();

    debug_output("process_command(): end of call");
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
