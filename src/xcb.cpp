
bool XcbConnection::establish(void)
{

    unsigned int data[1];

    this->connection = xcb_connect(NULL, NULL);

    if (xcb_connection_has_error(this->connection))
    {
        return false;
    }

    this->screen = xcb_setup_roots_iterator(
        xcb_get_setup(this->connection)
    ).data;

    /*
        Check for a running window manager here.
     */

    data[0] = {
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
        XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_ENTER_WINDOW |
        XCB_EVENT_MASK_PROPERTY_CHANGE
    };

    xcb_generic_error_t *xcb_error = xcb_request_check(
        this->connection,
        xcb_change_window_attributes_checked(
            this->connection,
            this->screen->root,
            custard::event_mask,
            data
        )
    );

    if (xcb_error != NULL)
    {
        free(xcb_error);
        return false;
    }

    data[0] = {
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
        XCB_EVENT_MASK_PROPERTY_CHANGE
    };

    xcb_change_window_attributes_checked(
        this->connection,
        this->screen->root,
        custard::event_mask,
        data
    );

    xcb_ungrab_key(
        this->connection,
        XCB_GRAB_ANY,
        this->screen->root,
        XCB_MOD_MASK_ANY
    );

    this->flush();

    return true;

}

bool XcbConnection::close(void)
{
    if (this->connection != NULL)
    {
        xcb_disconnect(this->connection);
        return true;
    }
    return false;
}

xcb_connection_t* XcbConnection::get_connection(void)
{
    return this->connection;
}

xcb_screen_t* XcbConnection::get_screen(void)
{
    return this->screen;
}

void XcbConnection::flush(void)
{
/*    xcb_flush(
        this->connection
    );*/

    xcb_aux_sync(
        this->connection
    );
}

void XcbConnection::start_event_loop(void)
{

    int event;

    while (custard::alive)
    {

        custard::generic_event = xcb_wait_for_event(this->connection);

        if (!custard::generic_event)
        {
            return;
        }

        event = custard::generic_event->response_type & ~0x80;
        Handlers::handle_event(event);

    }

}
