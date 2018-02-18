
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

        update_screen = Commands::parse(message);

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

