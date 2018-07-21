
typedef enum {
    UNMAPPED,
    MAPPED,
    FOCUSED
} group_state_t;
/* TODO: find a way to either dynamically declare the amount of groups, or have them statically set with a command line option */
group_state_t group_states[];
unsigned int focused_group = 0;

void attach_window_to_group(xcb_window_t, unsigned int);
void detach_window_from_group(xcb_window_t, unsigned int);
void focus_group(unsigned int);
void map_group(unsigned int);
void unmap_group(unsigned int);
