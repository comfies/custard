
bool EwmhConnection::establish(void)
{

    unsigned int data[1] = {
        XCB_EVENT_MASK_POINTER_MOTION
    };

    this->window = xcb_generate_id(
        custard::xcb_connection->get_connection()
    );

    xcb_create_window(
        custard::xcb_connection->get_connection(),
        XCB_COPY_FROM_PARENT,
        this->window,
        custard::xcb_connection->get_screen()->root,
        0,
        0,
        custard::xcb_connection->get_screen()->width_in_pixels,
        custard::xcb_connection->get_screen()->height_in_pixels,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        XCB_COPY_FROM_PARENT,
        custard::event_mask,
        data
    );

    this->connection = (xcb_ewmh_connection_t *)malloc(sizeof(
        xcb_ewmh_connection_t
    ));

    if (!this->connection)
    {
        return false;
    }

    if (!xcb_ewmh_init_atoms_replies(
        this->connection,
        xcb_ewmh_init_atoms(
            custard::xcb_connection->get_connection(),
            this->connection
        ),
        NULL
    ))
    {
        return false;
    }

    xcb_ewmh_set_wm_pid(
        this->connection,
        this->window,
        getpid()
    );

    xcb_ewmh_set_wm_name(
        this->connection,
        this->window,
        8,
        "custard"
    );

    xcb_ewmh_set_supporting_wm_check(
        this->connection,
        this->window,
        this->window
    );

    xcb_ewmh_set_supporting_wm_check(
        this->connection,
        custard::xcb_connection->get_screen()->root,
        this->window
    );

    xcb_atom_t supported_atoms[] = {
        this->connection->_NET_WM_NAME,
        this->connection->_NET_ACTIVE_WINDOW,
        this->connection->_NET_SUPPORTED,
        this->connection->_NET_SUPPORTING_WM_CHECK,
        this->connection->_NET_WM_PID,
        this->connection->_NET_WM_STATE,

        this->connection->_NET_WM_WINDOW_TYPE,
        this->connection->_NET_WM_WINDOW_TYPE_DOCK,
        this->connection->_NET_WM_WINDOW_TYPE_TOOLBAR,
        this->connection->_NET_WM_WINDOW_TYPE_MENU,
        this->connection->_NET_WM_WINDOW_TYPE_DROPDOWN_MENU,
        this->connection->_NET_WM_WINDOW_TYPE_POPUP_MENU,
        this->connection->_NET_WM_WINDOW_TYPE_DIALOG,
        this->connection->_NET_WM_WINDOW_TYPE_DESKTOP,
        this->connection->_NET_WM_WINDOW_TYPE_SPLASH,
        this->connection->_NET_WM_WINDOW_TYPE_DND,

        this->connection->WM_PROTOCOLS
    };

    xcb_ewmh_set_supported(
        this->connection,
        0,  // Screen number
        17, // supported atoms
        supported_atoms
    );

    return true;

}

bool EwmhConnection::close(void)
{
    xcb_ewmh_connection_wipe(this->connection);
    free(this->connection);

    return true;
}

xcb_window_t EwmhConnection::get_window(void)
{
    return this->window;
}

xcb_ewmh_connection_t* EwmhConnection::get_connection(void)
{
    return this->connection;
}
