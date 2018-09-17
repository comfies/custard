#include "custard.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>

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

unsigned int workspaces = 2;

struct GeometryLinkedListElement *geometry_list_head = NULL;

unsigned short
new_geometry(char *name, unsigned int x, unsigned int y,
    unsigned int height, unsigned int width) {

    /* Almost like window_list_append_window */

    struct GeometryLinkedListElement *element = geometry_list_head;
    struct Geometry *geometry = (struct Geometry *)malloc(
        sizeof(struct Geometry));

    geometry->name = (char *)malloc(sizeof(char));

    strcpy(geometry->name, name);
    geometry->x = x;
    geometry->y = y;
    geometry->height = height;
    geometry->width = width;

    if (!element) {
        debug_output("No head geometry, making head");

        geometry_list_head = (struct GeometryLinkedListElement *)malloc(
            sizeof(struct GeometryLinkedListElement));

        geometry_list_head->geometry = geometry;
        geometry_list_head->next = NULL;
    } else {
        debug_output("Head geometry found, making tail");

        while (element->next) {
            element = element->next;
        }

        struct GeometryLinkedListElement *next_element;
        next_element = (struct GeometryLinkedListElement *)malloc(
            sizeof(struct GeometryLinkedListElement));

        next_element->next = NULL;
        next_element->geometry = geometry;

        element->next = next_element;
    }

    debug_output("New geometry: %s (%d %d %d %d)",
        name, x, y, height, width);

    return 1;

}

unsigned short
clear_geometry_list() {
    struct GeometryLinkedListElement *element = geometry_list_head;
    struct GeometryLinkedListElement *old_element = NULL;

    while (element) {
        old_element = element;
        element = element->next;

        free(old_element->geometry->name);
        free(old_element->geometry);
        free(old_element);
    }

    return 1;
}
