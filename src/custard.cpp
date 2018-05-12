namespace custard {

    static void *start_fifo_read_loop(void*)
    {
        fifo->start_read_loop();
        return NULL;
    }

    static void setup_pre_existing_windows(void)
    {
        xcb_query_tree_reply_t *tree_reply;
        tree_reply = xcb_query_tree_reply(
            xcb_connection->get_connection(),
            xcb_query_tree(
                xcb_connection->get_connection(),
                xcb_connection->get_screen()->root
            ),
            NULL
        );

        if (tree_reply != NULL)
        {

            unsigned int data[2] = {
                XCB_EVENT_MASK_ENTER_WINDOW,
                XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
            };

            xcb_window_t *tree_children;
            tree_children = xcb_query_tree_children(tree_reply);

            xcb_window_t window_id;
            Window *window = NULL;

            for (unsigned int index = 0; index < xcb_query_tree_children_length(
                tree_reply
            ); index++)
            {
                window_id = tree_children[index];

                if (window_id == ewmh_connection->get_window())
                {
                    continue;
                }

                window = new Window(window_id);

                window->map();

                windows.push_back(window);

                if (window->is_managed())
                {
                    current_workspace->manage(window);
                }
            }

            if (window != NULL)
            {
                window->raise();
                window->focus();
                window->center_cursor();
            }

        }

        free(tree_reply);
    }

    static void stop(void)
    {

        if (!alive)
        {
            return;
        }

        alive = false;

        pthread_join(fifo_thread, NULL);

        fifo->close();
        std::cerr << " [custard] Fifo closed" << std::endl;
        ewmh_connection->close();
        std::cerr << " [custard] EWMH connection closed" << std::endl;
        xcb_connection->close();
        std::cerr << " [custard] XCB connection closed" << std::endl;

        free(fifo);
        free(ewmh_connection);
        free(xcb_connection);

    }

    static void start(void)
    {

        fifo = new Fifo();

        xcb_connection = new XcbConnection();
        ewmh_connection = new EwmhConnection();

        xcb_connection->establish();
        ewmh_connection->establish();

        Configuration::load();
        Grid::setup();

        Workspace *workspace;
        for (unsigned int index = 0; index < Configuration::workspaces; index++)
        {
            workspace = new Workspace();

            if (index == 0)
            {
                current_workspace = workspace;
                workspace->map();
            }

            workspaces.push_back(workspace);
        }

        xcb_ewmh_set_number_of_desktops(
            ewmh_connection->get_connection(),
            0,
            Configuration::workspaces
        );

        xcb_ewmh_set_current_desktop(
            ewmh_connection->get_connection(),
            0,
            0
        );

        setup_pre_existing_windows();

        Handlers::attach_event_handlers();

        alive = true;

        pthread_create(&fifo_thread, NULL, start_fifo_read_loop, NULL);

        xcb_warp_pointer(
            xcb_connection->get_connection(),
            XCB_NONE,
            ewmh_connection->get_window(),
            0, 0, 0, 0,
            xcb_connection->get_screen()->width_in_pixels / 2,
            xcb_connection->get_screen()->height_in_pixels / 2
        );

        xcb_ewmh_set_active_window(
            ewmh_connection->get_connection(),
            0,
            XCB_NONE
        );

        xcb_connection->flush();

        xcb_connection->start_event_loop();

        stop();

    }

    static Window* get_focused_window(void)
    {
        Window *window = NULL;
        for (unsigned int index = 0; index < windows.size(); index++)
        {
            window = windows.at(index);
            if (window->is_focused() && window->is_mapped())
            {
                return window;
            }
        }

        return NULL;
    }

    static Window* get_window(xcb_window_t id)
    {
        Window *window = NULL;

        for (unsigned int index = 0; index < windows.size(); index++)
        {
            window = windows.at(index);
            if (!window)
            {
                continue;
            }

            if (window->get_id() == id)
            {
                return window;
            }
        }

        return NULL;
    }

    static void reset_cursor(void)
    {
        xcb_warp_pointer(
            xcb_connection->get_connection(),
            XCB_NONE,
            ewmh_connection->get_window(),
            0, 0, 0, 0,
            xcb_connection->get_screen()->width_in_pixels,
            xcb_connection->get_screen()->height_in_pixels
        );
    }
    /*
        Cycle between windows
     */

    static void cycle_to_next(void)
    {
        Window *window = NULL;
        std::vector<Window*> mapped;

        unsigned int focused_window_index = 0;
        for (unsigned int index = 0; index < windows.size(); index++)
        {
            window = windows.at(index);

            if (!window->is_mapped() || !window->is_managed())
            {
                continue;
            }

            if (window->is_focused())
            {
                focused_window_index = mapped.size();
            }

            mapped.push_back(window);
        }

        if (mapped.size() == 0)
        {
            return;
        }

        unsigned int index = 0;
        if (focused_window_index < mapped.size() - 1)
        {
            index = focused_window_index + 1;
        }

        Window* previous_window = get_focused_window();
        window = mapped.at(index);

        reset_cursor();
        if (previous_window)
        {
            previous_window->set_focus_false();
        }
        window->focus();
        window->raise();
        window->center_cursor();
    }

    static void cycle_to_prev(void)
    {
        Window *window = NULL;
        std::vector<Window*> mapped;

        unsigned int focused_window_index = 0;
        for (unsigned int index = 0; index < windows.size(); index++)
        {
            window = windows.at(index);

            if (!window->is_mapped() || !window->is_managed())
            {
                continue;
            }

            if (window->is_focused())
            {
                focused_window_index = mapped.size();
            }

            mapped.push_back(window);
        }

        if (mapped.size() == 0)
        {
            return;
        }

        unsigned int index = 0;
        if (focused_window_index == 0)
        {
            index = mapped.size() - 1;
        }
        else
        {
            index = focused_window_index - 1;
        }

        Window* previous_window = get_focused_window();
        window = mapped.at(index);

        reset_cursor();
        if (previous_window)
        {
            previous_window->set_focus_false();
        }
        window->focus();
        window->raise();
        window->center_cursor();
    }

    /*
        Focus methods

     */

    static void focus_north(void)
    {
        Window *window = get_focused_window();
        unsigned int x = Grid::default_x;
        unsigned int y = Grid::default_y + 1;

        if (window)
        {
            x = window->get_x();
            y = window->get_y();
        }

        while (y >= 1)
        {
            y--;
        
            for (unsigned int index = 0; index < windows.size(); index++)
            {
                window = windows.at(index);
                if (!window->is_mapped() || !window->is_managed())
                {
                    continue;
                }

                if (window->get_x() == x && window->get_y() == y)
                {
                    window->focus();
                    window->raise();
                    window->center_cursor();
                    return;
                }
            }

            /* Ideally, these would be a single for loop, but that's
                not easily done, in this specific case.
             */

            for (unsigned int index = 0; index < windows.size(); index++)
            {
                window = windows.at(index);
                if (!window->is_mapped() || !window->is_managed())
                {
                    continue;
                }

                if (x <= window->get_x() + window->get_span_x() && window->get_y() == y)
                {
                    window->focus();
                    window->raise();
                    window->center_cursor();
                    return;
                }
            }

        }
    }

    static void focus_south(void)
    {
        Window *window = get_focused_window();
        unsigned int x = Grid::default_x;
        unsigned int y = Grid::default_y - 1;

        if (window)
        {
            x = window->get_x();
            y = window->get_y();
        }

        while (y <= Configuration::grid_rows)
        {
            y++;

            for (unsigned int index = 0; index < windows.size(); index++)
            {
                window = windows.at(index);
                if (!window->is_mapped() || !window->is_managed())
                {
                    continue;
                }

                if (window->get_x() == x && window->get_y() == y)
                {
                    window->focus();
                    window->raise();
                    window->center_cursor();
                    return;
                }
            }

            for (unsigned int index = 0; index < windows.size(); index++)
            {
                window = windows.at(index);
                if (!window->is_mapped() || !window->is_managed())
                {
                    continue;
                }

                if (x <= window->get_x() + window->get_span_x() && window->get_y() == y)
                {
                    window->focus();
                    window->raise();
                    window->center_cursor();
                    return;
                }
            }

        }
    }

    static void focus_west(void)
    {
        Window *window = get_focused_window();
        unsigned int x = Grid::default_x + 1;
        unsigned int y = Grid::default_y;

        if (window)
        {
            x = window->get_x();
            y = window->get_y();
        }

        while (x >= 1)
        {
            x--;

            for (unsigned int index = 0; index < windows.size(); index++)
            {
                window = windows.at(index);
                if (!window->is_mapped() || !window->is_managed())
                {
                    continue;
                }

                if (window->get_x() == x && window->get_y() == y)
                {
                    window->focus();
                    window->raise();
                    window->center_cursor();
                    return;
                }
            }

            for (unsigned int index = 0; index < windows.size(); index++)
            {
                window = windows.at(index);
                if (!window->is_mapped() || !window->is_managed())
                {
                    continue;
                }

                if (window->get_x() == x && y <= window->get_y() + window->get_span_y())
                {
                    window->focus();
                    window->raise();
                    window->center_cursor();
                    return;
                }
            }

        }
    }

    static void focus_east(void)
    {
        Window *window = get_focused_window();
        unsigned int x = Grid::default_x - 1;
        unsigned int y = Grid::default_y;

        if (window)
        {
            x = window->get_x();
            y = window->get_y();
        }

        while (x <= Configuration::grid_columns)
        {
            x++;

            for (unsigned int index = 0; index < windows.size(); index++)
            {
                window = windows.at(index);
                if (!window->is_mapped() || !window->is_managed())
                {
                    continue;
                }

                if (window->get_x() == x && window->get_y() == y)
                {
                    window->focus();
                    window->raise();
                    window->center_cursor();
                    return;
                }
            }

            for (unsigned int index = 0; index < windows.size(); index++)
            {
                window = windows.at(index);
                if (!window->is_mapped() || !window->is_managed())
                {
                    continue;
                }

                if (window->get_x() == x && y <= window->get_y() + window->get_span_y())
                {
                    window->focus();
                    window->raise();
                    window->center_cursor();
                    return;
                }
            }

        }
    }

    /*
        Workspace methods
     */

    static Workspace* get_workspace(Window *window)
    {
        Workspace *workspace;
        for (unsigned int index = 0; index < workspaces.size(); index++)
        {
            workspace = workspaces.at(index);
            if (workspace->manages(window))
            {
                return workspace;
            }
        }
        return NULL;
    }

    static Workspace* get_workspace(unsigned int n)
    {
        unsigned int index = n - 1;

        if (index >= workspaces.size())
        {
            return NULL;
        }

        return workspaces.at(index);
    }

    static void go_to_workspace(unsigned int n)
    {
        Workspace *workspace = get_workspace(n);

        if (workspace == NULL)
        {
            return;
        }

        if (workspace != current_workspace && workspace->num_managed_windows() >= 1)
        {

            // "Resets" the cursor to prevent window flickering

            reset_cursor();

        }

        Workspace *non_target;
        for (unsigned int index = 0; index < workspaces.size(); index++)
        {
            non_target = workspaces.at(index);
            if (non_target != workspace)
            {
                non_target->unmap();
            }
        }
        workspace->map();
        current_workspace = workspace;

        xcb_ewmh_set_current_desktop(
            ewmh_connection->get_connection(),
            0,
            n - 1
        );

    }

    static void attach_workspace(unsigned int n)
    {
        Workspace *workspace = get_workspace(n);

        if (workspace == NULL || workspace == current_workspace)
        {
            return;
        }

        workspace->map();

    }

    static void detach_workspace(unsigned int n)
    {
        Workspace *workspace = get_workspace(n);

        if (workspace == NULL || workspace == current_workspace)
        {
            return;
        }

        workspace->unmap();

    }

    static void send_focused_window_to_workspace(unsigned int n)
    {
        Workspace *target_workspace = get_workspace(n); // Target
        Window *window = get_focused_window();
        Workspace *original_workspace = get_workspace(window); // Original

        if (!target_workspace)
        {
            return;
        }

        if (target_workspace == original_workspace)
        {
            return;
        }

        if (!target_workspace->is_mapped())
        {
            window->unmap();
            if (window->is_focused())
            {
                window->set_focus_false(); // Prevents two windows from being focused
            }
        }

        original_workspace->unmanage(window);
        target_workspace->manage(window);

    }

}
