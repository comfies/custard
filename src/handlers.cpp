
namespace Handlers {

    std::map<int, void(*)()> handlers;

    /* Begin of handlers */

    static void window_map_request(void)
    {
        xcb_map_request_event_t *event;
        event = (xcb_map_request_event_t *)custard::generic_event;

        xcb_window_t window_id = event->window;

        Window *window = new Window(window_id);
        window->map();
        window->raise();
        window->focus();
        window->center_cursor();

        custard::windows.push_back(window);
        custard::current_workspace->manage(window);
    }

    static void window_on_hover(void)
    {
        xcb_enter_notify_event_t *event;
        event = (xcb_enter_notify_event_t *)custard::generic_event;

        xcb_window_t window_id = event->event;

        for (unsigned int index = 0; index < custard::windows.size(); index++)
        {
            Window *window = custard::windows.at(index);
            if (window->get_id() == window_id)
            {
                window->raise();
                window->focus();
                return;
            }
        }
    }

    static void window_destroyed(void)
    {
        xcb_destroy_notify_event_t *event;
        event = (xcb_destroy_notify_event_t *)custard::generic_event;

        xcb_window_t window_id = event->window;

        Window *window;
        for (unsigned int index = 0; index < custard::windows.size(); index++)
        {
            window = custard::windows.at(index);
            if (window->get_id() == window_id)
            {
                custard::windows.erase(custard::windows.begin() + index);
                custard::get_workspace(window)->unmanage(window);
                return;
            }
        }
    }

    /* End of handlers */

    static void attach_event_handler(int event, auto method)
    {
        if (!handlers[event])
            handlers[event] = method;
    }

    static void attach_event_handlers(void)
    {
        attach_event_handler(XCB_MAP_REQUEST, window_map_request);
        attach_event_handler(XCB_DESTROY_NOTIFY, window_destroyed);
        attach_event_handler(XCB_ENTER_NOTIFY, window_on_hover);
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

