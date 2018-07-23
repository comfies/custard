
group_state_t
get_group_state(unsigned int group)
{
    debug_output("get_group_state(): called");
    group_state_t state = (group_state_t)((groups & (1 << (group - 1))) % 2);

    if (state == MAPPED && focused_group == group) {
        state = FOCUSED;
    }

    return state;
}

void
attach_window_to_group(xcb_window_t window_id, unsigned int group) {
    debug_output("attach_window_to_group(): called");
    Window *window = window_list_get_window(window_id);

    if (window) {
        window->groups |= (1 << (group - 1));
    }

    group_state_t state = get_group_state(group);

    if (state == FOCUSED || state == MAPPED) {
        map_window(window->id);
    } else {
        unmap_window(window->id);
    }
}

void
detach_window_from_group(xcb_window_t window_id, unsigned int group) {
    debug_output("detach_window_to_group(): called");
    Window *window = window_list_get_window(window_id);

    if (window) {
        window->groups &= ~(1 << (group - 1));
    }
}

void
focus_group(unsigned int group) {
    debug_output("focus_group(): called");

    for (unsigned int index = 0; index < Configuration->groups; index++) {
        unmap_group(index + 1);
    }

    map_group(group);
}

void
map_group(unsigned int group) {
    debug_output("map_group(): called");
    struct WindowLinkedListElement *element = window_list_head;
    Window *window = NULL;
    unsigned int index = group - 1;

    while (element) {
        window = element->window;

        if (window->groups & (1 << index)) {
            map_window(window->id);
        }

        element = element->next;
    }
}

void
unmap_group(unsigned int group) {
    debug_output("unmap_group(): called");
    struct WindowLinkedListElement *element = window_list_head;
    Window *window = NULL;
    unsigned int index = group - 1;

    while (element) {
        window = element->window;

        if (window->groups & (1 << index)) {
            unmap_window(window->id);
        }

        element = element->next;
    }
}
