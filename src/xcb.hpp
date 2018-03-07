
class XcbConnection {
    public:
        bool establish(void);
        bool close(void);

        xcb_connection_t* get_connection(void);
        xcb_screen_t* get_screen(void);

        void flush(void);

        void start_event_loop(void);
    private:
        xcb_connection_t *connection;
        xcb_screen_t *screen;
};

