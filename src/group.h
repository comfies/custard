typedef enum {
    UNMAPPED,
    MAPPED,
    FOCUSED
} group_state_t;
short unsigned int groups = 0;
unsigned int focused_group = 1;

group_state_t get_group_state(unsigned int);

short unsigned int window_is_in_group(xcb_window_t, unsigned int);

void attach_window_to_group(xcb_window_t, unsigned int);
void detach_window_from_group(xcb_window_t, unsigned int);

void focus_group(unsigned int);
void map_group(unsigned int);
void unmap_group(unsigned int);
