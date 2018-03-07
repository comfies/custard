
Window::Window(xcb_window_t id)
{
    this->id = id;

    xcb_atom_t atom;
    xcb_ewmh_get_atoms_reply_t window_type;

    if (xcb_ewmh_get_wm_window_type_reply(
        custard::ewmh_connection->get_connection(),
        xcb_ewmh_get_wm_window_type(
            custard::ewmh_connection->get_connection(),
            this->id
        ),
        &window_type, NULL))
    {

        for (unsigned int index = 0; index < window_type.atoms_len; index++)
        {

            atom = window_type.atoms[index];

            if (atom == custard::ewmh_connection->get_connection()->_NET_WM_WINDOW_TYPE_TOOLBAR ||
                atom == custard::ewmh_connection->get_connection()->_NET_WM_WINDOW_TYPE_DND ||
                atom == custard::ewmh_connection->get_connection()->_NET_WM_WINDOW_TYPE_DOCK ||
                atom == custard::ewmh_connection->get_connection()->_NET_WM_WINDOW_TYPE_DESKTOP ||
                atom == custard::ewmh_connection->get_connection()->_NET_WM_WINDOW_TYPE_SPLASH ||
                atom == custard::ewmh_connection->get_connection()->_NET_WM_WINDOW_TYPE_NOTIFICATION)
            {
                xcb_ewmh_get_atoms_reply_wipe(&window_type);

                if (atom == custard::ewmh_connection->get_connection()->_NET_WM_WINDOW_TYPE_SPLASH)
                {
                    xcb_get_geometry_reply_t *geometry = this->get_geometry();

                    unsigned int width = geometry->width;
                    unsigned int height = geometry->height;

                    unsigned int x = (custard::xcb_connection->get_screen()->width_in_pixels - width) / 2;
                    unsigned int y = (custard::xcb_connection->get_screen()->height_in_pixels - height) / 2;

                    unsigned int position[2] = {x, y};

                    this->move(position);
                }
                else if (atom == custard::ewmh_connection->get_connection()->_NET_WM_WINDOW_TYPE_NOTIFICATION)
                {
                    this->always_on_top = true;
                }

                return;
            }

        }

    }

    xcb_get_window_attributes_reply_t *attributes = this->get_attributes();

    if (attributes)
    {
        if (attributes->override_redirect)
        {
            return;
        }
    }

    unsigned int data[2] = {
        XCB_EVENT_MASK_ENTER_WINDOW,
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
    };

    xcb_change_window_attributes(
        custard::xcb_connection->get_connection(),
        this->id,
        custard::event_mask,
        data
    );

    this->managed = true;

    this->move(Grid::default_x, Grid::default_y);
    this->resize(Grid::default_height, Grid::default_width);
    this->update_borders();
}

xcb_window_t Window::get_id(void)
{
    return this->id;
}

xcb_get_window_attributes_reply_t* Window::get_attributes(void)
{
    return xcb_get_window_attributes_reply(
        custard::xcb_connection->get_connection(),
        xcb_get_window_attributes(
            custard::xcb_connection->get_connection(),
            this->id
        ),
        NULL
    );
}

xcb_get_geometry_reply_t* Window::get_geometry(void)
{
    return xcb_get_geometry_reply(
        custard::xcb_connection->get_connection(),
        xcb_get_geometry(
            custard::xcb_connection->get_connection(),
            this->id
        ),
        NULL
    );
}

bool Window::is_managed(void)
{
    return this->managed;
}

bool Window::is_focused(void)
{
    return this->focused;
}

bool Window::is_mapped(void)
{
    return this->mapped;
}

bool Window::is_always_on_top(void)
{
    return this->always_on_top;
}

bool Window::is_fullscreen(void)
{
    return this->fullscreened;
}

void Window::focus(void)
{

    if (this->focused)
    {
        xcb_ewmh_set_active_window(
            custard::ewmh_connection->get_connection(),
            0,
            this->id
        ); // Ensure this window is the active window

        return;
    }

    Window *focused_window = NULL;
    focused_window = custard::get_focused_window();

    if (focused_window)
    {
        focused_window->set_focus_false();
    }

    this->focused = true;

    xcb_set_input_focus(
        custard::xcb_connection->get_connection(),
        XCB_INPUT_FOCUS_POINTER_ROOT,
        this->id,
        XCB_CURRENT_TIME
    );

    xcb_ewmh_set_active_window(
        custard::ewmh_connection->get_connection(),
        0,
        this->id
    );

    if (!this->fullscreened)
        this->update_borders();

}

void Window::set_focus_false(bool update_borders)
{
    if (!this->focused)
    {
        return;
    }

    this->focused = false;

    if (update_borders)
    {
        this->update_borders();
    }

    xcb_ewmh_set_active_window(
        custard::ewmh_connection->get_connection(),
        0,
        custard::xcb_connection->get_screen()->root
    );
}

void Window::center_cursor(void)
{
    xcb_get_geometry_reply_t *geometry = this->get_geometry();

    unsigned int x = geometry->width / 2;
    unsigned int y = geometry->height / 2;

    xcb_warp_pointer(
        custard::xcb_connection->get_connection(),
        XCB_NONE,
        this->id,
        0, 0, 0, 0,
        x, y
    );
}

void Window::resize(unsigned int* data)
{
    xcb_configure_window(
        custard::xcb_connection->get_connection(),
        this->id,
        XCB_CONFIG_WINDOW_HEIGHT | XCB_CONFIG_WINDOW_WIDTH,
        data
    );
}

void Window::resize(short unsigned int height, short unsigned int width)
{

    if (!this->managed)
    {
        return;
    }

    if (this->fullscreened)
    {
        this->window();
        this->update_borders();
    }

    this->span_x = width;
    this->span_y = height;

    unsigned int height_in_pixels = Grid::get_span_y(height);
    unsigned int width_in_pixels = Grid::get_span_x(width);

    unsigned int data[2] = {width_in_pixels, height_in_pixels};
    this->resize(data);
}

void Window::move(unsigned int* data)
{
    xcb_configure_window(
        custard::xcb_connection->get_connection(),
        this->id,
        XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
        data
    );
}

void Window::move(short unsigned int x, short unsigned int y)
{
    if (!this->managed)
    {
        return;
    }

    if (this->fullscreened)
    {
        this->window();
        this->update_borders();
    }

    this->x = x;
    this->y = y;

    unsigned int y_in_pixels = Grid::get_offset_y(y) + Configuration::screen_margin_top;
    unsigned int x_in_pixels = Grid::get_offset_x(x) + Configuration::screen_margin_left;

    unsigned int data[2] = {x_in_pixels, y_in_pixels};
    this->move(data);
}

void Window::raise(void)
{
    unsigned int data[1] = {XCB_STACK_MODE_ABOVE};
    xcb_configure_window(
        custard::xcb_connection->get_connection(),
        this->id,
        XCB_CONFIG_WINDOW_STACK_MODE,
        data
    );
}

void Window::map(void)
{
    this->mapped = true;
    xcb_map_window(
        custard::xcb_connection->get_connection(),
        this->id
    );
}

void Window::unmap(void)
{
    this->mapped = false;
    xcb_unmap_window(
        custard::xcb_connection->get_connection(),
        this->id
    );
}

void Window::fullscreen(void)
{
    if (this->fullscreened)
    {
            return;
    }

    this->fullscreened = true;

    unsigned int data[2] = {0, 0};

    this->move(data);

    data[0] = custard::xcb_connection->get_screen()->width_in_pixels;
    data[1] = custard::xcb_connection->get_screen()->height_in_pixels;

    this->resize(data);

    this->raise();
}

void Window::window(void)
{
    if (!this->fullscreened)
    {
        return;
    }

    this->fullscreened = false;

    this->move(this->x, this->y);
    this->resize(this->span_y, this->span_x);
}

void Window::remove_borders(void)
{
    xcb_get_geometry_reply_t *geometry = this->get_geometry();

    if (!geometry)
    {
        return;
    }

    unsigned int border_width[1] = {0};

    xcb_configure_window(
        custard::xcb_connection->get_connection(),
        this->id,
        XCB_CONFIG_WINDOW_BORDER_WIDTH,
        border_width
    );
}

void Window::update_borders(void)
{

    if (!this->managed)
    {
        return;
    }

    xcb_get_geometry_reply_t *geometry = this->get_geometry();

    if (!geometry)
    {
        std::cerr << " [window] Window (" << this->id << ") border update stopped; geometry NULL" << std::endl;
        return;
    }

    unsigned int border_width[1] = {Configuration::border_size * Configuration::border_type};

    std::cerr << " [window] Window (" << this->id << ") border update" << std::endl;

    xcb_configure_window(
        custard::xcb_connection->get_connection(),
        this->id,
        XCB_CONFIG_WINDOW_BORDER_WIDTH,
        border_width
    );

    if (Configuration::border_type == 1)
    {
        this->update_border_helper_1();
    }
    else if (Configuration::border_type == 2)
    {
        this->update_border_helper_2();
    }
    else if (Configuration::border_type == 3)
    {
        this->update_border_helper_3();
    }

    std::cerr << " [window] Window (" << this->id << ") border update finished" << std::endl;

}

void Window::update_border_helper_1(void)
{

    unsigned int background_color[1];

    if (this->focused)
    {
        if (Configuration::border_invert_colors)
        {
            background_color[0] = Configuration::border_unfocused_color;
        }
        else
        {
            background_color[0] = Configuration::border_focused_color;
        }
    }
    else
    {
        if (Configuration::border_invert_colors)
        {
            background_color[0] = Configuration::border_focused_color;
        }
        else
        {
            background_color[0] = Configuration::border_unfocused_color;
        }
    }

    xcb_change_window_attributes(
        custard::xcb_connection->get_connection(),
        this->id,
        XCB_CW_BORDER_PIXEL,
        background_color
    );

}

void Window::update_border_helper_2(void)
{

    xcb_get_geometry_reply_t *geometry = this->get_geometry();

    short unsigned int width = (short unsigned)geometry->width;
    short unsigned int height = (short unsigned)geometry->height;

    xcb_rectangle_t inner_border[5] = {
        {
            (short)width,
            0,
            (short unsigned)Configuration::border_size,
            (short unsigned)(height + Configuration::border_size)
        }, // Right
        {
            0,
            (short)height,
            (short unsigned)(width + Configuration::border_size),
            (short unsigned)Configuration::border_size
        }, // Bottom
        {
            (short)(width + (Configuration::border_size * 3)),
            0,
            (short unsigned)Configuration::border_size,
            (short unsigned)(height + Configuration::border_size)
        }, // Left
        {
            0,
            (short)(height + (Configuration::border_size * 3)),
            (short unsigned)(width + Configuration::border_size),
            (short unsigned)Configuration::border_size
        }, // Top
        {
            (short)(width + (Configuration::border_size * 3)),
            (short)(height + (Configuration::border_size * 3)),
            (short unsigned)Configuration::border_size,
            (short unsigned)Configuration::border_size
        } // Top-left corner
    };

    this->update_border_helper_2_3(inner_border, 5);

}

void Window::update_border_helper_3(void)
{

    xcb_get_geometry_reply_t *geometry = this->get_geometry();

    short unsigned int width = (short unsigned)geometry->width;
    short unsigned int height = (short unsigned)geometry->height;

    xcb_rectangle_t inner_border[8] = {
        {
            (short)(width + Configuration::border_size),
            0,
            (short unsigned)Configuration::border_size,
            (short unsigned)(height + (Configuration::border_size * 2))
        }, // Right
        {
            0,
            (short)(height + Configuration::border_size),
            (short unsigned)(width + (Configuration::border_size * 2)),
            (short unsigned)Configuration::border_size
        }, // Bottom
        {
            (short)(width + (Configuration::border_size * 4)),
            0,
            (short unsigned)Configuration::border_size,
            (short unsigned)(height + (Configuration::border_size * 2))
        }, // Left
        {
            (short)(width + (Configuration::border_size * 4)),
            (short)(height + (Configuration::border_size * 4)),
            (short unsigned)(Configuration::border_size * 2),
            (short unsigned)Configuration::border_size
        }, // Top-left corner (1)
        {
            0,
            (short)(height + (Configuration::border_size * 4)),
            (short unsigned)(width + (Configuration::border_size * 2)),
            (short unsigned)Configuration::border_size
        }, // Top
        {
            (short)(width + Configuration::border_size),
            (short)(height + (Configuration::border_size * 4)),
            (short unsigned)Configuration::border_size,
            (short unsigned)(Configuration::border_size * 2)
        }, // Top-right corner
        {
            (short)(width + (Configuration::border_size * 4)),
            (short)(height + (Configuration::border_size * 4)),
            (short unsigned)Configuration::border_size,
            (short unsigned)(Configuration::border_size * 2)
        }, // Top-left corner (2)
        {
            (short)(width + (Configuration::border_size * 4)),
            (short)(height + Configuration::border_size),
            (short unsigned)(Configuration::border_size * 2),
            (short unsigned)Configuration::border_size
        } // Bottom-left corner
    };

    this->update_border_helper_2_3(inner_border, 8);

}

void Window::update_border_helper_2_3(xcb_rectangle_t *inner_border, unsigned int inner_border_size)
{

    std::cerr << " [window] Window (" << this->id << ") border update interstitial" << std::endl;

    unsigned int data[1] = {Configuration::border_unfocused_color};
    unsigned int background_color[1] = {Configuration::border_background_color};

    if (Configuration::border_invert_colors)
    {
        if (this->focused)
        {
            background_color[0] = Configuration::border_focused_color;
        }
        else
        {
            background_color[0] = Configuration::border_unfocused_color;
        }

        data[0] = Configuration::border_background_color;
    }
    else
    {
        if (this->focused)
        {
            data[0] = Configuration::border_focused_color;
        }
    }

    xcb_get_geometry_reply_t *geometry = this->get_geometry();
    unsigned int border_size;

    if (Configuration::border_type == 2)
    {
        border_size = (Configuration::border_size * 2);
    }
    else if (Configuration::border_type == 3)
    {
        border_size = (Configuration::border_size * 3);
    }
    else
    {
        // How the fuck did you get here?
        return;
    }

    xcb_rectangle_t outer_border[4] = {
        0, 0,
        (unsigned short)(geometry->width + (border_size * 2)),
        (unsigned short)(geometry->height + (border_size * 2))
    };

    xcb_pixmap_t pixmap = xcb_generate_id(
        custard::xcb_connection->get_connection()
    );

    xcb_create_pixmap(
        custard::xcb_connection->get_connection(),
        custard::xcb_connection->get_screen()->root_depth,
        pixmap,
        custard::xcb_connection->get_screen()->root,
        (unsigned short)(geometry->width + (border_size * 2)),
        (unsigned short)(geometry->height + (border_size * 2))
    );

    xcb_gcontext_t gc = xcb_generate_id(
        custard::xcb_connection->get_connection()
    );

    xcb_create_gc(
        custard::xcb_connection->get_connection(),
        gc,
        pixmap,
        0,
        NULL
    );

    xcb_change_gc(
        custard::xcb_connection->get_connection(),
        gc,
        XCB_GC_FOREGROUND,
        background_color
    );

    xcb_poly_fill_rectangle(
        custard::xcb_connection->get_connection(),
        pixmap,
        gc,
        4,
        outer_border
    );

    xcb_change_gc(
        custard::xcb_connection->get_connection(),
        gc,
        XCB_GC_FOREGROUND,
        data
    );

    xcb_poly_fill_rectangle(
        custard::xcb_connection->get_connection(),
        pixmap,
        gc,
        inner_border_size,
        inner_border
    );

    data[0] = pixmap;

    xcb_change_window_attributes(
        custard::xcb_connection->get_connection(),
        this->id,
        XCB_CW_BORDER_PIXMAP,
        data
    );

    xcb_free_pixmap(
        custard::xcb_connection->get_connection(),
        pixmap
    );

    xcb_free_gc(
        custard::xcb_connection->get_connection(),
        gc
    );

}

void Window::close(void)
{


    this->focused = false;

    bool hard_kill = false;

    xcb_icccm_get_wm_protocols_reply_t protocols;
    xcb_get_property_cookie_t cookie = xcb_icccm_get_wm_protocols_unchecked(
        custard::xcb_connection->get_connection(),
        this->id,
        custard::ewmh_connection->get_connection()->WM_PROTOCOLS
    );

    xcb_intern_atom_cookie_t atom_cookie = xcb_intern_atom(
        custard::xcb_connection->get_connection(),
        0,
        16,
        "WM_DELETE_WINDOW"
    );

    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(
        custard::xcb_connection->get_connection(),
        atom_cookie,
        NULL
    );

    if (!reply)
    {
        hard_kill = true;
    }

    if (!hard_kill)
    {

        xcb_atom_t delete_window_atom = reply->atom;

        if (xcb_icccm_get_wm_protocols_reply(
            custard::xcb_connection->get_connection(),
            cookie,
            &protocols,
            NULL
        ) == 1)
        {
            for (unsigned int index = 0; index < protocols.atoms_len; index++)
            {
                if (protocols.atoms[index] == delete_window_atom)
                {
                    hard_kill = true;
                    break;
                }
            }
        }

        if (!hard_kill)
        {

            xcb_client_message_event_t event = {
                .response_type = XCB_CLIENT_MESSAGE,
                .format = 32,
                .sequence = 0,
                .window = this->id,
                .type = custard::ewmh_connection->get_connection()->WM_PROTOCOLS,
                .data = {
                    .data32 = {
                        delete_window_atom,
                        XCB_CURRENT_TIME
                    }
                }
            };

            xcb_send_event(
                custard::xcb_connection->get_connection(),
                false,
                this->id,
                XCB_EVENT_MASK_NO_EVENT,
                (char *)&event
            );
        }

    }

    if (hard_kill)
    {
        xcb_kill_client(
            custard::xcb_connection->get_connection(),
            this->id
        );
    }

    custard::xcb_connection->flush();

}

void Window::maximize(void)
{

    if (this->fullscreened)
    {
        return;
    }

    custard::reset_cursor();
    this->resize(Configuration::grid_rows, Configuration::grid_columns);
    this->move(1, 1);

    this->raise();
    this->center_cursor();
    this->update_borders();
}

void Window::minimize(void)
{

    if (this->fullscreened)
    {
        return;
    }

    custard::reset_cursor();
    this->resize(Grid::default_height, Grid::default_width);
    this->move(Grid::default_x, Grid::default_y);

    this->raise();
    this->center_cursor();
    this->update_borders();
}

/* Misc */

bool Window::move_up(void)
{
    if (this->y == 1)
    {
        return false;
    }

    this->y--;
    custard::reset_cursor();
    this->move(this->x, this->y);
    this->raise();
    this->center_cursor();

    return true;
}

bool Window::move_down(void)
{
    if (this->y == Configuration::grid_rows || this->y + this->span_y > Configuration::grid_rows)
    {
        return false;
    }

    this->y++;
    custard::reset_cursor();
    this->move(this->x, this->y);
    this->raise();
    this->center_cursor();

    return true;
}

bool Window::move_left(void)
{
    if (this->x == 1)
    {
        return false;
    }

    this->x--;
    custard::reset_cursor();
    this->move(this->x, this->y);
    this->raise();
    this->center_cursor();

    return true;
}

bool Window::move_right(void)
{
    if (this->x == Configuration::grid_columns || this->x + this->span_x > Configuration::grid_columns)
    {
        return false;
    }

    this->x++;
    custard::reset_cursor();
    this->move(this->x, this->y);
    this->raise();
    this->center_cursor();

    return true;
}

bool Window::grow_up(void)
{

    if ((this->y - 1) == 0)
    {
        return false;
    }

    this->y--;
    this->span_y++;

    custard::reset_cursor();
    this->resize(this->span_y, this->span_x);
    this->move(this->x, this->y);

    this->raise();
    this->center_cursor();
    this->update_borders();

    return true;
}

bool Window::grow_down(void)
{

    if (this->y + (this->span_y + 1) > (Configuration::grid_rows + 1))
    {
        return false;
    }

    this->span_y++;
    custard::reset_cursor();
    this->resize(this->span_y, this->span_x);

    this->raise();
    this->center_cursor();
    this->update_borders();

    return true;

}

bool Window::grow_left(void)
{

    if ((this->x - 1) == 0)
    {
        return false;
    }

    this->x--;
    this->span_x++;

    custard::reset_cursor();
    this->resize(this->span_y, this->span_x);
    this->move(this->x, this->y);

    this->raise();
    this->center_cursor();
    this->update_borders();

    return true;

}

bool Window::grow_right(void)
{

    if (this->x + (this->span_x + 1) > (Configuration::grid_columns + 1))
    {
        return false;
    }

    this->span_x++;
    custard::reset_cursor();
    this->resize(this->span_y, this->span_x);

    this->raise();
    this->center_cursor();
    this->update_borders();

    return true;
}

bool Window::shrink_up(void)
{

    if ((this->span_y - 1) == 0)
    {
        return false;
    }

    this->span_y--;
    custard::reset_cursor();
    this->resize(this->span_y, this->span_x);

    this->raise();
    this->center_cursor();
    this->update_borders();

    return true;

}

bool Window::shrink_down(void)
{

    if ((this->span_y - 1) == 0)
    {
        return false;
    }

    this->span_y--;
    this->y++;

    custard::reset_cursor();
    this->resize(this->span_y, this->span_x);
    this->move(this->x, this->y);

    this->raise();
    this->center_cursor();
    this->update_borders();

    return true;

}

bool Window::shrink_left(void)
{

    if ((this->span_x - 1) == 0)
    {
        return false;
    }

    this->span_x--;
    custard::reset_cursor();
    this->resize(this->span_y, this->span_x);

    this->raise();
    this->center_cursor();
    this->update_borders();

    return true;
}

bool Window::shrink_right(void)
{
    if ((this->span_x - 1) == 0)
    {
        return false;
    }

    this->span_x--;
    this->x++;

    custard::reset_cursor();
    this->move(this->x, this->y);
    this->resize(this->span_y, this->span_x);

    this->raise();
    this->center_cursor();
    this->update_borders();

    return true;
}
