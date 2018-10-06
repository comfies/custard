
class Window {
    public:
        xcb_window_t get_id(void);

        xcb_get_window_attributes_reply_t* get_attributes(void);
        xcb_get_geometry_reply_t* get_geometry(void);

        bool is_managed(void);
        bool is_focused(void);
        bool is_mapped(void);
        bool is_always_on_top(void);
        bool is_fullscreen(void);

        unsigned int get_x(void);
        unsigned int get_y(void);
        unsigned int get_span_x(void);
        unsigned int get_span_y(void);

        void focus(void);
        void set_focus_false(bool = true);

        void center_cursor(void);

        void resize(unsigned int*);
        void move(unsigned int*);

        void resize(short unsigned int, short unsigned int);
        void move(short unsigned int, short unsigned int);

        void raise(void);
        void lower(void);

        void remove_borders(void);
        void update_borders(void);

        void close(void);

        void map(void);
        void unmap(void);

        void maximize(void);
        void minimize(void);

        void fullscreen(void);
        void window(void);

        bool move_north(void);
        bool move_south(void);
        bool move_west(void);
        bool move_east(void);

        bool grow_north(void);
        bool grow_south(void);
        bool grow_west(void);
        bool grow_east(void);

        bool shrink_north(void);
        bool shrink_south(void);
        bool shrink_west(void);
        bool shrink_east(void);

        Window(xcb_window_t);
    private:
        unsigned int x;
        unsigned int y;
        unsigned int span_x;
        unsigned int span_y;

        bool mapped = false;
        bool focused = false;
        bool preventfocus = false;
        bool always_on_top = false;

        xcb_window_t id;

        bool managed = false;

        bool fullscreened = false;

        void update_border_helper_1(void);
        void update_border_helper_2(void);
        void update_border_helper_3(void);
        void update_border_helper_2_3(xcb_rectangle_t*, unsigned int);
};

