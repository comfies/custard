
class Window {
    public:
        xcb_window_t get_id(void);

        xcb_get_window_attributes_reply_t* get_attributes(void);
        xcb_get_geometry_reply_t* get_geometry(void);

        bool is_managed(void);
        bool is_focused(void);
        bool is_mapped(void);
        bool is_always_on_top(void);

        void focus(void);
        void set_focus_false(bool = true);

        void center_cursor(void);

	void resize(unsigned int*);
        void move(unsigned int*);

        void resize(short unsigned int, short unsigned int);
        void move(short unsigned int, short unsigned int);

        void raise(void);

        void update_borders(void);

        void close(void);

        void map(void);
        void unmap(void);

        void maximize(void);
        void unmaximize(void);

        void fullscreen(void);
        void window(void);

        bool move_up(void);
        bool move_down(void);
        bool move_left(void);
        bool move_right(void);

        bool grow_up(void);
        bool grow_down(void);
        bool grow_left(void);
        bool grow_right(void);

        bool shrink_up(void);
        bool shrink_down(void);
        bool shrink_left(void);
        bool shrink_right(void);

        Window(xcb_window_t);
    private:
        unsigned int x;
        unsigned int y;
        unsigned int span_x;
        unsigned int span_y;

        bool mapped = false;
        bool focused = false;
        bool always_on_top = false;

        xcb_window_t id;

        bool managed = false;

        bool maximized = false;
        bool fullscreened = false;

        void update_border_helper_1(void);
        void update_border_helper_2(void);
        void update_border_helper_3(void);
        void update_border_helper_2_3(xcb_rectangle_t*, unsigned int);
};

