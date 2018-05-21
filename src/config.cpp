
namespace Configuration {

    /* User config */

    short unsigned int grid_rows = 2;
    short unsigned int grid_columns = 3;

    short unsigned int border_type = 0;

    unsigned int inner_border_size = 0;
    unsigned int outer_border_size = 0;

    unsigned int border_focused_color = 0xFFFFFF;
    unsigned int border_unfocused_color = 0x676767;
    unsigned int border_background_color = 0x000000;

    bool border_invert_colors = false;

    unsigned int grid_gap_size = 0;

    unsigned int screen_margin_top = 0;
    unsigned int screen_margin_bottom = 0;
    unsigned int screen_margin_left = 0;
    unsigned int screen_margin_right = 0;

    unsigned int workspaces = 2;

    /* End user config */

    static void apply_defaults(void)
    {
        if (!grid_rows)
        {
            grid_rows = 2;
        }

        if (!grid_columns)
        {
            grid_columns = 3;
        }

        if (!border_focused_color)
        {
            border_focused_color = 0xFFFFFF;
        }

        if (!border_unfocused_color)
        {
            border_unfocused_color = 0x676767;
        }

        if (!border_background_color)
        {
            border_background_color = 0x000000;
        }

        if (!grid_gap_size)
        {
            grid_gap_size = 0;
        }

        if (!screen_margin_top)
        {
            screen_margin_top = 0;
        }

        if (!screen_margin_bottom)
        {
            screen_margin_bottom = 0;
        }

        if (!screen_margin_left)
        {
            screen_margin_left = 0;
        }

        if (!screen_margin_right)
        {
            screen_margin_right = 0;
        }

        /* placeholder */

        if (!workspaces)
        {
            workspaces = 2;
        }
    }

    static unsigned int parse_uint(const char* str)
    {
        if (str)
        {
            return atoi(str);
        }
        return 0;
    }

    static bool parse_bool(const char* str)
    {
        if (str)
        {
            if (strcmp(str, "True") == 0 || strcmp(str, "true") == 0)
            {
                return true;
            }
            else if (strcmp(str, "False") == 0 || strcmp(str, "false") == 0)
            {
                return false;
            } // Not needed, but I like having it written explicitly.

            return false;
        }

        return false;
    }

    static unsigned int parse_color(const char* hexadecimal_notation)
    {
        if (!hexadecimal_notation)
        {
            return 0;
        }

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

    static char* get_setting(xcb_xrm_database_t *database, std::string name)
    {
        std::string setting = "custard." + name;

        char *str;
        if (xcb_xrm_resource_get_string(database,
            setting.c_str(), NULL, &str) >= 0)
        {
            return str;
        }

        return NULL;
    }

    static void load(void)
    {

        xcb_xrm_database_t *database = xcb_xrm_database_from_default(
            custard::xcb_connection->get_connection()
        );

        if (!database)
        {
            return;
        }

        grid_rows = parse_uint(get_setting(database, "rows"));
        grid_columns = parse_uint(get_setting(database, "columns"));

        // Colors

        border_focused_color = parse_color(
                                get_setting(database, "focused_color"));

        border_unfocused_color = parse_color(
                                get_setting(database, "unfocused_color"));

        border_background_color = parse_color(
                                get_setting(database, "background_color"));

        border_invert_colors = parse_bool(
                                get_setting(database, "invert_colors"));

        // Borders

        char *temporary = get_setting(database, "border_type");

        if (temporary)
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

        inner_border_size = parse_uint(get_setting(database, "inner_border_size"));
        outer_border_size = parse_uint(get_setting(database, "outer_border_size"));

        // Margins

        grid_gap_size = parse_uint(get_setting(database, "margin"));

        screen_margin_top = parse_uint(get_setting(database, "margin_top"));
        screen_margin_bottom = parse_uint(
                                get_setting(database, "margin_bottom"));
        screen_margin_left = parse_uint(get_setting(database, "margin_left"));
        screen_margin_right = parse_uint(
                                get_setting(database, "margin_right"));

        // Groups

        workspaces = parse_uint(get_setting(database, "workspaces"));

        xcb_xrm_database_free(database);
        apply_defaults();

    }

}
