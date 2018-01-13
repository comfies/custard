
namespace Grid {

    unsigned int unit_pixel_size_x;
    unsigned int unit_pixel_size_y;

    unsigned int border_size;

    short unsigned int default_x = 0;
    short unsigned int default_y = 0;

    short unsigned int default_height = 1;
    short unsigned int default_width = 1;

    static void setup(void)
    {

        border_size = Configuration::border_size * Configuration::border_type;

        unit_pixel_size_y = (custard::xcb_connection->get_screen()->height_in_pixels - (
            (Configuration::grid_gap_size * (Configuration::grid_rows + 1)) + (
                border_size * (Configuration::grid_rows * 2)
            ) + (
                Configuration::screen_margin_top +
                Configuration::screen_margin_bottom
            )
        )) / Configuration::grid_rows;

        unit_pixel_size_x = (custard::xcb_connection->get_screen()->width_in_pixels - (
            (Configuration::grid_gap_size * (Configuration::grid_columns + 1)) + (
                border_size * (Configuration::grid_columns * 2)
            ) + (
                Configuration::screen_margin_left +
                Configuration::screen_margin_right
            )
        )) / Configuration::grid_columns;

        /*
            Columns     -> X
            Rows        -> Y
         */

        if (Configuration::grid_rows % 2 == 0)
        {
            default_y = (Configuration::grid_rows / 2);
            default_height = 2;
        }
        else
        {
            default_y = (Configuration::grid_rows / 2) + 1;
        }

        if (Configuration::grid_columns % 2 == 0)
        {
            default_x = (Configuration::grid_columns / 2);
            default_width = 2;
        }
        else
        {
            default_x = (Configuration::grid_columns / 2) + 1;
        }

    }

    static unsigned int get_span(unsigned int unit_pixels, unsigned int span_amount)
    {
        return (unit_pixels * span_amount) + (
            Configuration::grid_gap_size * (span_amount - 1)
        ) + (
            (border_size * 2) * (span_amount - 1)
        );
    }

    static unsigned int get_span_x(unsigned int span_amount)
    {
        return get_span(unit_pixel_size_x, span_amount);
    }

    static unsigned int get_span_y(unsigned int span_amount)
    {
        return get_span(unit_pixel_size_y, span_amount);
    }

    static unsigned int get_offset(unsigned int unit_pixels, unsigned int position)
    {
        unsigned int position_index = position - 1;
        return (
            Configuration::grid_gap_size * (position_index + 1)
        ) + (unit_pixels * (position_index)) + (
            border_size * ((position_index) * 2)
        );
    }

    static unsigned int get_offset_x(unsigned int position)
    {
        return get_offset(unit_pixel_size_x, position);
    }

    static unsigned int get_offset_y(unsigned int position)
    {
        return get_offset(unit_pixel_size_y, position);
    }

}

