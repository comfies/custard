
namespace Configuration {

    /* User config */

    short unsigned int grid_rows = 2;
    short unsigned int grid_columns = 3;

    short unsigned int border_type = 0;
    unsigned int border_size = 0;

    unsigned int border_focused_color = 0xFFFFFF;
    unsigned int border_unfocused_color = 0x676767;
    unsigned int border_background_color = 0x000000;

    bool border_invert_colors = false;

    unsigned int grid_gap_size = 0;

    unsigned int screen_margin_top = 0;
    unsigned int screen_margin_bottom = 0;
    unsigned int screen_margin_left = 0;
    unsigned int screen_margin_right = 0;

    unsigned int workspaces = 1;

    /* End user config */

    unsigned int parse_color(const char* hexadecimal_notation)
    {
        unsigned int RGB;
        char string_groups[7] = {
            hexadecimal_notation[1],
            hexadecimal_notation[2],
            hexadecimal_notation[3],
            hexadecimal_notation[4],
            hexadecimal_notation[5],
            hexadecimal_notation[6],
            '\0'
        };

        RGB = strtol(string_groups, NULL, 16);

        return RGB;
    }

    static void load(void)
    {

        xcb_xrm_database_t *database = xcb_xrm_database_from_default(
            custard::xcb_connection->get_connection()
        );

        if (database == NULL)
        {
            return;
        }

        char* temporary;

        if (xcb_xrm_resource_get_string(
            database, "custard.rows", NULL, &temporary) >= 0)
        {
            grid_rows = atoi(temporary);
        }

        if (xcb_xrm_resource_get_string(
            database, "custard.columns", NULL, &temporary) >= 0)
        {
            grid_columns = atoi(temporary);
        }

        // Colors

        if (xcb_xrm_resource_get_string(
            database, "custard.focused_color", NULL, &temporary) >= 0)
        {
            border_focused_color = parse_color(temporary);
        }

        if (xcb_xrm_resource_get_string(
            database, "custard.unfocused_color", NULL, &temporary) >= 0)
        {
            border_unfocused_color = parse_color(temporary);
        }

        if (xcb_xrm_resource_get_string(
            database, "custard.background_color", NULL, &temporary) >= 0)
        {
            border_background_color = parse_color(temporary);
        }

        if (xcb_xrm_resource_get_string(
            database, "custard.invert_colors", NULL, &temporary) >= 0)
        {
            if (strcmp(
                temporary, "True") == 0 || strcmp(
                temporary, "true") == 0)
            {
                border_invert_colors = true;
            }
        }

        // Borders

        if (xcb_xrm_resource_get_string(
            database, "custard.border_type", NULL, &temporary) >= 0)
        {

            if (strcmp(
                temporary, "Triple") == 0 || strcmp(
                temporary, "triple") == 0)
            {
                border_type = 3;
            }
            else if (
                strcmp(
                    temporary, "Double") == 0 || strcmp(
                    temporary, "double") == 0
            )
            {
                border_type = 2;
            }
            else if (
                strcmp(
                    temporary, "Single") == 0 || strcmp(
                    temporary, "single") == 0
            )
            {
                border_type = 1;
            }

        }

        if (xcb_xrm_resource_get_string(
            database, "custard.border_size", NULL, &temporary) >= 0)
        {
            border_size = atoi(temporary);
        }

        // Margins

        if (xcb_xrm_resource_get_string(
            database, "custard.margin", NULL, &temporary) >= 0)
        {
            grid_gap_size = atoi(temporary);
        }

        if (xcb_xrm_resource_get_string(
            database, "custard.margin_top", NULL, &temporary) >= 0)
        {
            screen_margin_top = atoi(temporary);
        }

        if (xcb_xrm_resource_get_string(
            database, "custard.margin_bottom", NULL, &temporary) >= 0)
        {
            screen_margin_bottom = atoi(temporary);
        }

        if (xcb_xrm_resource_get_string(
            database, "custard.margin_left", NULL, &temporary) >= 0)
        {
            screen_margin_left = atoi(temporary);
        }

        if (xcb_xrm_resource_get_string(
            database, "custard.margin_right", NULL, &temporary) >= 0)
        {
            screen_margin_right = atoi(temporary);
        }

        // Groups

        if (xcb_xrm_resource_get_string(
            database, "custard.workspaces", NULL, &temporary) >= 0)
        {
            workspaces = atoi(temporary);
        }

        xcb_xrm_database_free(database);

    }

}
