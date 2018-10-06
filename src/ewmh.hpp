
class EwmhConnection {
    public:
        bool establish(void);
        bool close(void);
        xcb_window_t get_window(void);
        xcb_ewmh_connection_t* get_connection(void);
    private:
        xcb_ewmh_connection_t *connection;
        xcb_window_t window;
};

