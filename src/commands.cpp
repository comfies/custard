namespace Commands {

    static bool target_custard(unsigned int command, unsigned int argument)
    {
        switch (command)
        {
            case 0:
                // stop
                custard::stop();
                break;

            case 1:
                // cycle
                if (argument == 0)
                {
                    custard::cycle_to_prev();
                }
                else if (argument == 1)
                {
                    custard::cycle_to_next();
                }

                return true;

                break;
        }

        return false;
    }

    static bool target_window(unsigned int command, unsigned int argument)
    {
        Window *window = custard::get_focused_window();
        if (!window)
        {
            return false;
        }

        switch (command)
        {
            case 0:
                // move
                if (argument == 0)
                {
                    // up
                    window->move_north();
                }
                else if (argument == 1)
                {
                    // down
                    window->move_south();
                }
                else if (argument == 2)
                {
                    // left
                    window->move_west();
                }
                else if (argument == 3)
                {
                    // right
                    window->move_east();
                }

                return true;
                break;

            case 1:
                if (argument == 0)
                {
                    window->grow_north();
                }
                else if (argument == 1)
                {
                    window->grow_south();
                }
                else if (argument == 2)
                {
                    window->grow_west();
                }
                else if (argument == 3)
                {
                    window->grow_east();
                }

                return true;
                break;

            case 2:
                if (argument == 0)
                {
                    window->shrink_north();
                }
                else if (argument == 1)
                {
                    window->shrink_south();
                }
                else if (argument == 2)
                {
                    window->shrink_west();
                }
                else if (argument == 3)
                {
                    window->shrink_east();
                }

                return true;
                break;

            case 3:

                custard::send_focused_window_to_workspace(argument);

                return true;
                break;

            case 4:
                window->close();

                return true;
                break;

            case 5:
                window->maximize();

                return true;
                break;

            case 6:
                window->minimize();

                return true;
                break;
        }

        return false;
    }

    static bool target_workspace(unsigned int command, unsigned int argument)
    {

        switch (command)
        {
            case 0:
                custard::attach_workspace(argument);

                return true;
                break;

            case 1:
                custard::detach_workspace(argument);

                return true;
                break;

            case 2:
                custard::go_to_workspace(argument);

                return true;
                break;
        }

        return false;

    }

    static bool parse(std::string command)
    {
        std::regex action("\\+(\\d+)\\.(\\d+)\\.(\\d+)\\;");
        std::smatch matches;

        if (std::regex_search(command, matches, action))
        {
            unsigned int target = atoi(matches[1].str().c_str());
            unsigned int command = atoi(matches[2].str().c_str());
            unsigned int argument = atoi(matches[3].str().c_str());

            switch (target)
            {
                case 0:
                    return target_custard(command, argument);
                    break;

                case 1:
                    return target_window(command, argument);
                    break;

                case 2:
                    return target_workspace(command, argument);
                    break;
            }

        }

        return false;
    }

}
