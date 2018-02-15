
namespace custard {

    Fifo *fifo;
    XcbConnection *xcb_connection;
    EwmhConnection *ewmh_connection;

    bool alive = false;

    xcb_generic_event_t *generic_event = NULL;
    int event_mask = XCB_CW_EVENT_MASK;

    std::vector<Window*> windows;

    std::vector<Workspace*> workspaces;
    Workspace *current_workspace = NULL;

    pthread_t fifo_thread;

    static void *start_fifo_read_loop(void);
    static void setup_pre_existing_windows(void);
    static void start(void);
    static Window* get_focused_window(void);
    static Window* get_window(xcb_window_t);
    static void stop(void);

    static void reset_cursor(void);

    static void cycle_to_next(void);
    static void cycle_to_prev(void);

    static Workspace* get_workspace(Window*);
    static Workspace* get_workspace(unsigned int);
    static void go_to_workspace(unsigned int);
    static void attach_workspace(unsigned int);
    static void detach_workspace(unsigned int);
    static void send_focused_window_to_workspace(unsigned int);

}

