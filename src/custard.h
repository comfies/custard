#define event_mask XCB_CW_EVENT_MASK

typedef struct Window Window;

short unsigned int debug = 0;
short unsigned int wm_running = 0;

pthread_t socket_thread;

struct WindowLinkedListElement {
    struct WindowLinkedListElement *next;
    Window *window;
};

struct WindowLinkedListElement *window_list_head = NULL;
Window *focused_window;

typedef enum {
    NORTH,
    SOUTH,
    WEST,
    EAST,
    NORTHWEST,
    NORTHEAST,
    SOUTHWEST,
    SOUTHEAST,
    ALL
} cardinal_direction_t;

unsigned int window_list_get_size(void);
short unsigned int window_list_append_window(Window*);
short unsigned int window_list_remove_window(xcb_window_t);
Window *window_list_get_window(xcb_window_t);

static void *debug_output(char* message);

int start_custard(void);
void stop_custard(void);
