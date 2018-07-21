
void
process_command(char *input) {
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

    if (!diced[0]) {
        return;
    }

    if (strcmp(diced[0], "custard") == 0) {
        if (!diced[1]) {
            return;
        }

        if (strcmp(diced[1], "halt") == 0) {
            debug_output("process_command(): halt called");
            stop_custard();
        } else if (strcmp(diced[1], "configure") == 0) {
            debug_output("process_command(): configure called");

            if (!diced[2] || !diced[3]) {
                return;
            }

            if (strcmp(diced[2], "debug_mode") == 0) {
                debug = parse_boolean(diced[3]);
            } else if (strcmp(diced[2], "border_focused_color") == 0) {
                Configuration->border_focused_color = parse_rgba_color(
                    diced[3]);
            } else if (strcmp(diced[2], "border_unfocused_color") == 0) {
                Configuration->border_unfocused_color = parse_rgba_color(
                    diced[3]);
            } else if (strcmp(diced[2], "border_background_color") == 0) {
                Configuration->border_background_color = parse_rgba_color(
                    diced[3]);
            } else if (strcmp(diced[2], "border_inner_size") == 0) {
                Configuration->border_inner_size = parse_unsigned_integer(
                    diced[3]);
            } else if (strcmp(diced[2], "border_outer_size") == 0) {
                Configuration->border_outer_size = parse_unsigned_integer(
                    diced[3]);
            } else if (strcmp(diced[2], "border_type") == 0) {
                Configuration->border_type = parse_unsigned_integer(diced[3]);
                if (Configuration->border_type > 3) {
                    Configuration->border_type = 3;
                }
            } else if (strcmp(diced[2], "grid_rows") == 0) {
                Configuration->grid_rows = parse_unsigned_integer(diced[3]);
            } else if (strcmp(diced[2], "grid_columns") == 0) {
                Configuration->grid_columns = parse_unsigned_integer(diced[3]);
            } else if (strcmp(diced[2], "grid_gap") == 0) {
                Configuration->grid_gap = parse_unsigned_integer(diced[3]);
            } else if (strcmp(diced[2], "grid_margin_top") == 0) {
                Configuration->grid_margin_top = parse_unsigned_integer(
                    diced[3]);
            } else if (strcmp(diced[2], "grid_margin_bottom") == 0) {
                Configuration->grid_margin_bottom = parse_unsigned_integer(
                    diced[3]);
            } else if (strcmp(diced[2], "grid_margin_left") == 0) {
                Configuration->grid_margin_left = parse_unsigned_integer(
                    diced[3]);
            } else if (strcmp(diced[2], "grid_margin_right") == 0) {
                Configuration->grid_margin_right = parse_unsigned_integer(
                    diced[3]);
            } else if (strcmp(diced[2], "groups") == 0) {
                Configuration->groups = parse_unsigned_integer(diced[2]);
            } else {
                return;
            }

            apply_config();
        } else {
            return;
        }

    } else if (strcmp(diced[0], "window") == 0) {
        if (!diced[1]) {
            return;
        }

        if (!focused_window) {
            return;
        }

        xcb_window_t window_id = focused_window->id;

        if (strcmp(diced[1], "close") == 0) {
            close_window(window_id);
        } else if (strcmp(diced[1], "raise") == 0) {
            raise_window(window_id);
        } else if (strcmp(diced[1], "lower") == 0) {
            lower_window(window_id);
        } else if (strcmp(diced[1], "move") == 0 ||
            strcmp(diced[1], "expand") == 0 ||
            strcmp(diced[1], "contract") == 0) {
            if (!diced[2]) {
                return;
            }

            cardinal_direction_t direction = (
                (cardinal_direction_t)parse_unsigned_integer(
                    diced[2]));
            if (strcmp(diced[1], "move") == 0) {
                move_window_cardinal(window_id, direction);
            } else if (strcmp(diced[1], "expand") == 0) {
                expand_window_cardinal(window_id, direction);
            } else if (strcmp(diced[1], "contract") == 0) {
                contract_window_cardinal(window_id, direction);
            } else {
                return;
            }

        } else if (strcmp(diced[1], "attach_to_group") == 0 ||
            strcmp(diced[1], "detach_from_group")) {
            if (!diced[2]) {
                return;
            }

            unsigned int group = parse_unsigned_integer(diced[2]);

            if (strcmp(diced[1], "attach_to_group") == 0) {
//                attach_window_to_group(window_id, group);
            } else if (strcmp(diced[1], "detach_from_group") == 0) {
//                detach_window_from_group(window_id, group);
            } else {
                return;
            }
        } else {
            return;
        }

    } else if (strcmp(diced[0], "group") == 0) {
        /* TODO: this */
    } else {
        return;
    }

    commit();

    debug_output("process_command(): end of call");
}

unsigned short int
parse_boolean(const char *input) {
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
parse_unsigned_integer(const char *input) {
    if (input) {
        return atoi(input);
    }

    return 0;
}

unsigned int
parse_rgba_color(const char *input) {
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
