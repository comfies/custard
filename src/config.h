struct Config {
    short unsigned int border_type;
    short unsigned int border_invert_colors;
    unsigned int border_inner_size;
    unsigned int border_outer_size;
    unsigned int border_total_size;
    unsigned int border_focused_color;
    unsigned int border_unfocused_color;
    unsigned int border_background_color;

    unsigned int grid_rows;
    unsigned int grid_columns;
    unsigned int grid_gap;
    unsigned int grid_margin_top;
    unsigned int grid_margin_bottom;
    unsigned int grid_margin_left;
    unsigned int grid_margin_right;

    unsigned int groups;
};

struct Config *Configuration;
void apply_config_defaults(void);
void apply_config(void);
