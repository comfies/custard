
/* Public members */

bool Workspace::manages(Window *window)
{
    return this->get_window_index(window) > -1;
}

bool Workspace::manage(Window *window)
{
    if (this->manages(window))
    {
        return false;
    }

    this->windows.push_back(window);

    return true;
}

bool Workspace::unmanage(Window *window)
{
    if (!this->manages(window))
    {
        return false;
    }

    this->windows.erase(
        this->windows.begin() + this->get_window_index(
            window
        )
    );

    return true;
}

bool Workspace::is_mapped(void)
{
    return this->mapped;
}

void Workspace::map(void)
{
    if (this->mapped)
    {
        return;
    }

    Window *window;
    for (unsigned int index = 0; index < this->windows.size(); index++)
    {
        window = this->windows.at(index);
        if (!window->is_managed())
        {
            continue;
        }
        window->map();
        window->raise();
    }

    this->mapped = true;
}

void Workspace::unmap(void)
{
    if (!this->mapped)
    {
        return;
    }

    Window *window;
    for (unsigned int index = 0; index < this->windows.size(); index++)
    {
        window = this->windows.at(index);
        if (!window->is_managed())
        {
            continue;
        }
        window->unmap();
        window->set_focus_false();
    }

    this->mapped = false;
}

/* Private members */

int Workspace::get_window_index(Window *window)
{
    Window *managed_window;
    for (unsigned int index = 0; index < this->windows.size(); index++)
    {
        managed_window = this->windows.at(index);
        if (managed_window->get_id() == window->get_id())
        {
            return index;
        }
    }

    return -1;
}
