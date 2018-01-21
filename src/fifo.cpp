
Fifo::Fifo(void)
{
    std::string path = "/tmp/custard.fifo";
    this->path = path.c_str();

    remove(this->path);
    mkfifo(this->path, 0666);

    this->fifo_fd = open(this->path, O_RDWR);

}

void Fifo::close(void)
{
    ::close(this->fifo_fd);
    remove(this->path);
}

void Fifo::start_read_loop(void)
{
    std::string message;
    bool update_screen = false;
    Window *window;

    while (true)
    {
        if (update_screen)
        {
            custard::xcb_connection->flush();
            update_screen = false;
        }

        message = this->read();

        window = custard::get_focused_window();

        if (message.find("reload config") != std::string::npos)
        {
            Configuration::load();
            Grid::setup();

            Window *window;

            for (unsigned int index = 0; index < custard::windows.size(); index++)
            {
                window = custard::windows.at(index);
                window->update_borders();
            }

            update_screen = true;
            /* TODO: reload window sizes */
        }
        else if (message.find("stop wm") != std::string::npos)
        {
            custard::stop();
        }
        else if (message.find("cycle focus forward") != std::string::npos)
        {
            custard::cycle_to_next();
            update_screen = true;
        }
        else if (message.find("cycle focus backward") != std::string::npos)
        {
            custard::cycle_to_prev();
            update_screen = true;
        }
        else
        {

            if (message.find("move window up") != std::string::npos && window != NULL)
            {
                window->move_up();
                update_screen = true;
            }
            else if (message.find("move window down") != std::string::npos && window != NULL)
            {
                window->move_down();
                update_screen = true;
            }
            else if (message.find("move window left") != std::string::npos && window != NULL)
            {
                window->move_left();
                update_screen = true;
            }
            else if (message.find("move window right") != std::string::npos && window != NULL)
            {
                window->move_right();
                update_screen = true;
            }
            else if (message.find("grow window up") != std::string::npos && window != NULL)
            {
                window->grow_up();
                update_screen = true;
            }
            else if (message.find("grow window down") != std::string::npos && window != NULL)
            {
                window->grow_down();
                update_screen = true;
            }
            else if (message.find("grow window left") != std::string::npos && window != NULL)
            {
                window->grow_left();
                update_screen = true;
            }
            else if (message.find("grow window right") != std::string::npos && window != NULL)
            {
                window->grow_right();
                update_screen = true;
            }
            else if (message.find("shrink window up") != std::string::npos && window != NULL)
            {
                window->shrink_up();
                update_screen = true;
            }
            else if (message.find("shrink window down") != std::string::npos && window != NULL)
            {
                window->shrink_down();
                update_screen = true;
            }
            else if (message.find("shrink window left") != std::string::npos && window != NULL)
            {
                window->shrink_left();
                update_screen = true;
            }
            else if (message.find("shrink window right") != std::string::npos && window != NULL)
            {
                window->shrink_right();
                update_screen = true;
            }
            else if (message.find("close window") != std::string::npos && window != NULL)
            {
                window->close();
                // No need to set `update_screen` here, Window::close calls custard::xcb_connection->flush
                // We don't want to unnecessarily update the screen
            }
            else
            {

                // Not exactly sure why I did this the way I did, but oh well

                std::regex action("(go to|send to|attach|detach) workspace (\\d+)");
                std::smatch match;

                if (std::regex_search(message, match, action))
                {
                    unsigned int nth_workspace = std::stoi(match[2]);
                    std::string action = match[1];

                    Workspace *workspace = custard::get_workspace(nth_workspace);

                    if (workspace == NULL)
                    {
                        continue;
                    }

                    if (action == "go to")
                    {
                        custard::go_to_workspace(nth_workspace);
                        update_screen = true;
                    }
                    else if (action == "send to" && window != NULL)
                    {
                        custard::send_focused_window_to_workspace(nth_workspace);
                        update_screen = true;
                    }
                    else if (action == "attach")
                    {
                        custard::attach_workspace(nth_workspace);
                        update_screen = true;
                    }
                    else if (action == "detach")
                    {
                        custard::detach_workspace(nth_workspace);
                        update_screen = true;
                    }

                }

            }

        }

    }
}

void Fifo::write(std::string message)
{
    const char* data = message.c_str();

    ::write(this->fifo_fd, data, 1024);
}

std::string Fifo::read(void)
{
    char data[1024];

    ::read(this->fifo_fd, data, 1024);

    return std::string(data);
}

