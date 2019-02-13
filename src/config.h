#ifndef CONFIG_H
#define CONFIG_H

#define SOCKET_PATH "/tmp/custard.sock"

extern short unsigned border_type;
extern short unsigned border_invert_colors;

extern unsigned int border_inner_size;
extern unsigned int border_outer_size;
extern unsigned int border_total_size;

extern unsigned int border_focused_color;
extern unsigned int border_unfocused_color;
extern unsigned int border_background_color;

extern unsigned int grid_rows;
extern unsigned int grid_columns;

extern unsigned int grid_gap;
extern unsigned int grid_margin_top;
extern unsigned int grid_margin_bottom;
extern unsigned int grid_margin_left;
extern unsigned int grid_margin_right;

extern unsigned int num_workspaces;

struct Geometry {
    char *name;
    unsigned int x;
    unsigned int y;
    unsigned int height;
    unsigned int width;
};

typedef enum {
    WINDOW_FIRST_CLASS,
    WINDOW_SECOND_CLASS,
    WINDOW_NAME
} window_attribute_t;

struct RuleData {
    unsigned int workspace;
    unsigned short sticky;
    char *geometry;
}; /* TODO: advanced rulings? See L52 */

struct GeometryRule {
    window_attribute_t attribute_type;
    char *match;
    char *geometry;
/*    struct RuleData*; */
};

extern struct LinkedListElement *geometry_list_head;
extern struct LinkedListElement *geometry_rules_list_head;

unsigned short new_geometry(const char*, unsigned int, unsigned int,
    unsigned int, unsigned int); /* Potential memory leak: be sure to free allocated pointers at custard end*/
unsigned short new_geometry_rule(window_attribute_t, char*, char*);

unsigned short initialize_configuration(void);
unsigned short finalize_configuration(void);

#endif /* CONFIG_H */
