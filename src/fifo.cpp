
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

    while (custard::alive)
    {
        if (update_screen)
        {
            custard::xcb_connection->flush();
            update_screen = false;
        }

        message = this->read();

        // expect an update for this again soon
        std::regex action("(custard)( (stop)| (cycle focus) (forward|backward))|(window)( (move|grow|shrink) (up|down|left|right)| (go to workspace) (\\d+)| (close))|(workspace)( (attach|detach|focus) (\\d+))\\;");
        std::smatch matches;

        window = custard::get_focused_window();

        std::cerr << " [fifo] command received" << std::endl;

        if (std::regex_search(message, matches, action))
        {

            if (matches[1] == "custard")
            {
                if (matches[3] == "stop")
                {
                    custard::stop();
                }
                else if (matches[4] == "cycle focus")
                {
                    if (matches[5] == "forward")
                    {
                        custard::cycle_to_next();
                    }
                    else if (matches[5] == "backward")
                    {
                        custard::cycle_to_prev();
                    }

                    update_screen = true;
                }
            }
            else if (matches[6] == "window")
            {
                if (!window)
                {
                    continue;
                }

                if (matches[8] == "move")
                {
                    if (matches[9] == "up")
                    {
                        window->move_up();
                    }
                    else if (matches[9] == "down")
                    {
                        window->move_down();
                    }
                    else if (matches[9] == "left")
                    {
                        window->move_left();
                    }
                    else if (matches[9] == "right")
                    {
                        window->move_right();
                    }
                }
                else if (matches[8] == "grow")
                {
                    if (matches[9] == "up")
                    {
                        window->grow_up();
                    }
                    else if (matches[9] == "down")
                    {
                        window->grow_down();
                    }
                    else if (matches[9] == "left")
                    {
                        window->grow_left();
                    }
                    else if (matches[9] == "right")
                    {
                        window->grow_right();
                    }
                }
                else if (matches[8] == "shrink")
                {
                    if (matches[9] == "up")
                    {
                        window->shrink_up();
                    }
                    else if (matches[9] == "down")
                    {
                        window->shrink_down();
                    }
                    else if (matches[9] == "left")
                    {
                        window->shrink_left();
                    }
                    else if (matches[9] == "right")
                    {
                        window->shrink_right();
                    }
                }
                else if (matches[10] == "go to workspace")
                {
                    unsigned int nth_workspace = std::stoi(matches[11]);

                    custard::send_focused_window_to_workspace(nth_workspace);
                }
                else if (matches[12] == "close")
                {
                    window->close();
                }

                update_screen = true;
            }
            else if (matches[13] == "workspace")
            {

                unsigned int nth_workspace = std::stoi(matches[16]);

                if (matches[15] == "attach")
                {
                    custard::attach_workspace(nth_workspace);
                }
                else if (matches[15] == "detach")
                {
                    custard::detach_workspace(nth_workspace);
                }
                else if (matches[15] == "focus")
                {
                    custard::go_to_workspace(nth_workspace);
                }

                update_screen = true;

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

