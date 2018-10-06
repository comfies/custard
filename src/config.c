#include "custard.h"
#include "config.h"
#include "ipc.h" /* TODO: move parse methods to another file */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libconfig.h>

short unsigned border_type = 0;
short unsigned border_invert_colors = 0;

unsigned int border_inner_size = 0;
unsigned int border_outer_size = 0;
unsigned int border_total_size = 0;

unsigned int border_focused_color = 0xFFFFFF;
unsigned int border_unfocused_color = 0x676767;
unsigned int border_background_color = 0x000000;

unsigned grid_rows = 2;
unsigned grid_columns = 2;

unsigned int grid_gap = 0;
unsigned int grid_margin_top = 0;
unsigned int grid_margin_bottom = 0;
unsigned int grid_margin_left = 0;
unsigned int grid_margin_right = 0;

unsigned int num_workspaces = 2;

struct LinkedListElement *geometry_list_head = NULL;
struct LinkedListElement *geometry_rules_list_head = NULL;

unsigned short
new_geometry(const char *name, unsigned int x, unsigned int y,
    unsigned int height, unsigned int width) {

    /* Almost like window_list_append_window */

    struct LinkedListElement *element = geometry_list_head;
    struct Geometry *geometry = (Geometry *)malloc(
        sizeof(Geometry));

    geometry->name = (char *)malloc(sizeof(char));

    strcpy(geometry->name, name);
    geometry->x = x;
    geometry->y = y;
    geometry->height = height;
    geometry->width = width;

    if (!element) {
        debug_output("No head geometry, making head");

        geometry_list_head = (struct LinkedListElement *)malloc(
            sizeof(struct LinkedListElement));

        geometry_list_head->data = geometry;
        geometry_list_head->next = NULL;
    } else {
        debug_output("Head geometry found, making tail");

        while (element->next) {
            element = element->next;
        }

        struct LinkedListElement *next_element;
        next_element = (struct LinkedListElement *)malloc(
            sizeof(struct LinkedListElement));

        next_element->next = NULL;
        next_element->data = geometry;

        element->next = next_element;
    }

    debug_output("New geometry: %s (%d %d %d %d)",
        name, x, y, height, width);

    return 1;

}

unsigned short
new_geometry_rule(window_attribute_t attribute, char *match, char *geometry) {

    struct LinkedListElement *element = geometry_rules_list_head;

    struct GeometryRule *rule = (struct GeometryRule *)malloc(
        sizeof(struct GeometryRule));

    rule->match = (char *)malloc(sizeof(char));
    rule->geometry = (char *)malloc(sizeof(char));

    strcpy(rule->match, match);
    strcpy(rule->geometry, geometry);

    rule->attribute_type = attribute;

    if (!element) {
        geometry_rules_list_head = (struct LinkedListElement *)malloc(
            sizeof(struct LinkedListElement));

        geometry_rules_list_head->data = rule;
        geometry_rules_list_head->next = NULL;
    } else {
        while (element->next) {
            element = element->next;
        }

        struct LinkedListElement *next_element;
        next_element = (struct LinkedListElement *)malloc(
            sizeof(struct LinkedListElement));

        next_element->data = rule;
        next_element->next = NULL;

        element->next = next_element;
    }

    return 1;
}

unsigned short
initialize_configuration() {
    config_t configuration;
    config_setting_t *setting;
    const char *rc_file;

    int temp; /* Hacky, but it works */

    debug_output("Initializing configuration.");

    config_init(&configuration);

    if (!config_path || !config_read_file(&configuration, config_path)) {
        debug_output("%s %s\n",
            "Unable to initialize configuration.",
            "Either the file does not exist or there is a syntax error.");
        config_destroy(&configuration);
        return 0;
    }

    config_lookup_bool(&configuration, "debug", &temp);
    debug = (short unsigned)temp;


    config_lookup_int(&configuration, "grid.rows", &temp);
    grid_rows = (unsigned)temp;

    config_lookup_int(&configuration, "grid.columns", &temp);
    grid_columns = (unsigned)temp;

    config_lookup_int(&configuration, "grid.gap_size", &temp);
    grid_gap = (unsigned)temp;

    config_lookup_int(&configuration, "grid.margins.top", &temp);
    grid_margin_top = (unsigned)temp;
    config_lookup_int(&configuration, "grid.margins.bottom", &temp);
    grid_margin_bottom = (unsigned)temp;
    config_lookup_int(&configuration, "grid.margins.left", &temp);
    grid_margin_left = (unsigned)temp;
    config_lookup_int(&configuration, "grid.margins.right", &temp);
    grid_margin_right = (unsigned)temp;

    config_lookup_int(&configuration, "border.sizes.inner", &temp);
    border_inner_size = (unsigned)temp;
    config_lookup_int(&configuration, "border.sizes.outer", &temp);
    border_outer_size = (unsigned)temp;

    const char *border_type_string;

    config_lookup_string(&configuration, "border.type", &border_type_string);

    if (strcmp(border_type_string, "single") == 0 ||
        strcmp(border_type_string, "Single") == 0) {
        border_type = 1;
    } else if (strcmp(border_type_string, "double") == 0 ||
        strcmp(border_type_string, "Double") == 0) {
        border_type = 2;
    } else if (strcmp(border_type_string, "triple") == 0 ||
        strcmp(border_type_string, "Triple") == 0) {
        border_type = 3;
    }

    if (border_type == 0) {
        border_total_size = 0;
    } else {
        border_total_size = border_inner_size + 
            (border_outer_size * (border_type - 1));
    }

    const char *color;

    if (config_lookup_string(&configuration, "border.colors.focused",
        &color)) {
        border_focused_color = parse_rgba_color(color);
    }

    if (config_lookup_string(&configuration, "border.colors.unfocused",
        &color)) {
        border_unfocused_color = parse_rgba_color(color);
    }

    if (config_lookup_string(&configuration, "border.colors.background",
        &color)) {
        border_background_color = parse_rgba_color(color);
    }

    config_lookup_bool(&configuration, "border.colors.invert", &temp);
    border_invert_colors = (short unsigned)temp;

    int index;

    setting = config_lookup(&configuration, "geometries");

    if (setting) {
        config_setting_t *geometry;

        for (index = 0; index < config_setting_length(setting); ++index) {
            geometry = config_setting_get_elem(setting, index);

            const char *name;
            int x, y, height, width;

            if (!(config_setting_lookup_string(geometry, "name", &name) &&
                config_setting_lookup_int(geometry, "x", &x) &&
                config_setting_lookup_int(geometry, "y", &y) &&
                config_setting_lookup_int(geometry, "height", &height) &&
                config_setting_lookup_int(geometry, "width", &width)
                )) {
                continue;
            }

            new_geometry(name, x, y, height, width);
        }
    }

    /* TODO: implement window rules */


    config_lookup_int(&configuration, "workspaces", &temp);
    num_workspaces = (unsigned)temp;

    /* Execute the RC */
    config_lookup_string(&configuration, "rc", &rc_file);

    debug_output("RC: %s\n", rc_file);

    if (rc_file) {
        debug_output("Executing %s\n", rc_file);

        if (fork() == 0) {
            execl(rc_file, rc_file, NULL);

            debug_output("Unable to execute %s\n", rc_file);
        }
    }

    config_destroy(&configuration);

    return 1;
}

unsigned short
finalize_configuration() {
    struct LinkedListElement *element = geometry_rules_list_head;
    struct LinkedListElement *old_element = NULL;

    while (element) {
        old_element = element;
        element = element->next;

        free(((Geometry *)old_element->data)->name);
        free(old_element->data);
        free(old_element);
    }

    element = geometry_rules_list_head;
    old_element = NULL;

    while (element) {
        old_element = element;
        element = element->next;

        free(((struct GeometryRule *)old_element->data)->match);
        free(((struct GeometryRule *)old_element->data)->geometry);
        free(old_element->data);
        free(old_element);
    }

    return 1;
}
