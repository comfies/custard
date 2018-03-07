
namespace Handlers {

    std::map<int, void(*)()> handlers;

    /* Begin of handlers */

    static void window_map_request(void)
    {
        xcb_map_request_event_t *event;
        event = (xcb_map_request_event_t *)custard::generic_event;

        xcb_window_t window_id = event->window;

        std::cerr << " [handler] Window (" << window_id << ") map request." << std::endl;

        Window *window = custard::get_window(window_id);
        if (window)
        {
            return;
        }

        window = new Window(window_id);
        window->map();
        window->focus();

        window->raise();
        window->center_cursor();

        custard::windows.push_back(window);

        if (window->is_managed())
        {
            custard::current_workspace->manage(window);
        }
    }

    static void window_on_hover(void)
    {
        xcb_enter_notify_event_t *event;
        event = (xcb_enter_notify_event_t *)custard::generic_event;

        Window *window = NULL;
        xcb_window_t window_id = event->event;

        std::cerr << " [handler] Window (" << window_id << ") hovered" << std::endl;

        window = custard::get_focused_window();
        if (window)
        {
            if (window->get_id() == window_id)
            {
                return;
            }
        }

        for (unsigned int index = 0; index < custard::windows.size(); index++)
        {
            window = custard::windows.at(index);
            if (!window)
            {
                continue;
            }

            if (window->get_id() == window_id)
            {
                window->raise();
                window->focus();
            }
            else
            {
                if (window->is_mapped())
                {
                    window->set_focus_false();
                }
            }
        }
        // Prefer this to custard::get_window, for `set_focus_false`
    }

    static void window_destroyed(void)
    {
        xcb_destroy_notify_event_t *event;
        event = (xcb_destroy_notify_event_t *)custard::generic_event;

        xcb_window_t window_id = event->window;

        std::cerr << " [handler] Window (" << window_id << ") destroyed" << std::endl;

        Window *window = NULL;

        Window *next_focus_target = NULL;
        for (unsigned int index = 0; index < custard::windows.size(); index++)
        {
            window = custard::windows.at(index);
            if (!window->is_mapped() || !window->is_managed() || window->get_id() == window_id)
            {
                continue;
            }

            next_focus_target = window;
        }

        bool mapped = false;

        window = NULL;
        for (unsigned int index = 0; index < custard::windows.size(); index++)
        {
            window = custard::windows.at(index);
            if (window->get_id() == window_id)
            {
                if (window->is_focused())
                {
                    window->set_focus_false(false);
                }

                if (window->is_mapped())
                {
                    mapped = true;
                }

                custard::windows.erase(custard::windows.begin() + index);

                Workspace *workspace = NULL;
                workspace = custard::get_workspace(window);
                if (workspace)
                {
                    workspace->unmanage(window);
                }

                free(window);

                if (mapped && next_focus_target)
                {
                    custard::reset_cursor();
                    next_focus_target->raise();
                    next_focus_target->focus();
                    next_focus_target->center_cursor();
                }

                return;
            }
        }
    }

    static void window_message_received(void)
    {
        xcb_client_message_event_t *event;
        event = (xcb_client_message_event_t *)custard::generic_event;

        Window *window = custard::get_window(event->window);

        if (!window)
        {
            return;
        }


        if (event->type == custard::ewmh_connection->get_connection()->_NET_WM_STATE)
        {

            if (event->data.data32[1] == custard::ewmh_connection->get_connection()->_NET_WM_STATE_FULLSCREEN ||
                event->data.data32[2] == custard::ewmh_connection->get_connection()->_NET_WM_STATE_FULLSCREEN)
            {
                    if (window->is_fullscreen())
                    {
                        window->window();
                        window->update_borders();
                    }
                    else
                    {
                        window->remove_borders();
                        window->fullscreen();
                    }
            }

        }
    }

    static void window_configure_request(void)
    {
        xcb_configure_request_event_t *event;
        event = (xcb_configure_request_event_t *)custard::generic_event;

        xcb_window_t window_id = event->window;
        Window *window = custard::get_window(window_id);

        if (!window)
        {
            return;
        }

        if (window->is_managed())
        {
            return; // May as well combine this and the above condition
        }

        unsigned int data[2];

        // This is a tasteless implementation, I should change it sometime.

        if (event->value_mask & XCB_CONFIG_WINDOW_X &&
            event->value_mask & XCB_CONFIG_WINDOW_Y)
        {
            data[0] = event->x;
            data[1] = event->y;

            window->move(data);
            data[0] = 0;
            data[1] = 0;
        }
        else
        {
            xcb_get_geometry_reply_t *geometry = window->get_geometry();

            if (event->value_mask & XCB_CONFIG_WINDOW_X)
            {
                data[0] = event->x;
                data[1] = geometry->y;

                window->move(data);
                data[0] = 0;
                data[1] = 0;
            }
            else if (event->value_mask & XCB_CONFIG_WINDOW_Y)
            {
                data[0] = geometry->x;
                data[1] = event->y;

                window->move(data);
                data[0] = 0;
                data[1] = 0;
            }
        }

        if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT &&
            event->value_mask & XCB_CONFIG_WINDOW_WIDTH)
        {
            data[0] = event->width;
            data[1] = event->height;

            window->resize(data);
            data[0] = 0;
            data[1] = 0;
        }
        else
        {
            xcb_get_geometry_reply_t *geometry = window->get_geometry();

            if (event->value_mask & XCB_CONFIG_WINDOW_WIDTH)
            {
                data[0] = event->width;
                data[1] = geometry->height;

                window->resize(data);
                data[0] = 0;
                data[1] = 0;
            }
            else if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT)
            {
                data[0] = geometry->width;
                data[1] = event->height;

                window->resize(data);
                data[0] = 0;
                data[1] = 0;
            }
        }
    }

    /* End of handlers */

    static void attach_event_handler(int event, auto method)
    {
        if (!handlers[event])
        {
            handlers[event] = method;
        }
    }

    static void attach_event_handlers(void)
    {
        attach_event_handler(XCB_MAP_REQUEST, window_map_request);
        attach_event_handler(XCB_DESTROY_NOTIFY, window_destroyed);
/*        attach_event_handler(XCB_ENTER_NOTIFY, window_on_hover);*/
        attach_event_handler(XCB_CLIENT_MESSAGE, window_message_received);
        attach_event_handler(XCB_CONFIGURE_REQUEST, window_configure_request);
    }

    static void handle_event(int event)
    {
        if (handlers[event])
        {
            handlers[event]();
            custard::xcb_connection->flush();
        }
    }

}

