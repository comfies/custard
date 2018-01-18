namespace custard {

    static void* start_fifo_read_loop(void*)
    {
        fifo->start_read_loop();
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

        pthread_join(fifo_thread, NULL);

        fifo->close();
        ewmh_connection->close();
        xcb_connection->close();

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

        pthread_create(&fifo_thread, NULL, start_fifo_read_loop, NULL);

        xcb_warp_pointer(
            xcb_connection->get_connection(),
            XCB_NONE,
            ewmh_connection->get_window(),
            0, 0, 0, 0,
            xcb_connection->get_screen()->width_in_pixels / 2,
            xcb_connection->get_screen()->height_in_pixels / 2
        );

        xcb_connection->flush();

        xcb_connection->start_event_loop();

        stop();

    }

    static Window* get_focused_window(void)
    {
        Window *window;
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

            xcb_warp_pointer(
                xcb_connection->get_connection(),
                XCB_NONE,
                ewmh_connection->get_window(),
                0, 0, 0, 0,
                xcb_connection->get_screen()->width_in_pixels,
                xcb_connection->get_screen()->height_in_pixels
            );

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
