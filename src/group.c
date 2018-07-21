
void
attach_window_to_group(xcb_window_t window_id, unsigned int group) {
    Window *window = window_list_get_window(window_id);

    if (window) {
        window->groups[group - 1] = 1;
    }

    /* TODO: check if group is mapped, and map window if it is? */
}

void
detach_window_from_group(xcb_window_t window_id, unsigned int group) {
    Window *window = window_list_get_window(window_id);

    if (window) {
        window->groups[group - 1] = 0;
    }
}

void
focus_group(unsigned int group) {
    for (unsigned int index = 0; index < Configuration->groups; index++) {
        if (group == (index + 1)) {
            continue;
        }

        unmap_group(index + 1);
    }

    map_group(group);
}

void
map_group(unsigned int group) {
    struct WindowLinkedListElement *element = window_list_head;
    Window *window = NULL;
    unsigned int index = group - 1;

    while (element) {
        window = element->window;

        if (window->groups[index] == 1) {
            map_window(window->id);
        }

        element = element->next;
    }
}

void
unmap_group(unsigned int group) {
    struct WindowLinkedListElement *element = window_list_head;
    Window *window = NULL;
    unsigned int index = group - 1;

    while (element) {
        window = element->window;

        if (window->groups[index] == 1) {
            unmap_window(window->id);
        }

        element = element->next;
    }
}
