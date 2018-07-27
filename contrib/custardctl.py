#!/usr/bin/env python3

import getopt
import re
import socket
import sys

dry = False

def show_help():
    print("[...] show help")
    exit(0)

def error_out(message):
    print(message)
    exit(1)

def send_message(message):
    message = message.lower()
    if dry:
        print(message)
        exit(0)
    socket_path = '/tmp/custard.sock'
    try:
        open_socket = socket.socket(socket.AF_UNIX)
        open_socket.settimeout(1)
        open_socket.connect(socket_path)
        open_socket.send(str(message).encode())
        open_socket.close()
        exit(0)
    except (ConnectionRefusedError, FileNotFoundError):
        error_out("Unable to connect to UNIX socket; " + \
            "custard may not be running")

# parse ...

def parse_boolean(user_input):
    if user_input in ['True', 'true']:
        return True
    elif user_input in ['False', 'false']:
        return False
    return False

def parse_unsigned_integer(user_input):
    if user_input.isdigit():
        return int(user_input)
    return 0

def parse_rgba_color(user_input):
    return user_input # ... for now

# main

if __name__ == '__main__':
    if len(sys.argv[1:]) == 0:
        show_help()

    targets = [
        'custard',
        'window',
        'group'
    ]

    target = 'custard'

    command_line_arguments = sys.argv[1:]
    if command_line_arguments[0] in targets:
        target = command_line_arguments[0]
        command_line_arguments = command_line_arguments[1:]
    else:
        if command_line_arguments[0][0:2] != '--':
            error_out("Invalid target specified: " + command_line_arguments[0])

    option_target_map = {
        'configure': ['custard'],
        'help': ['custard'],
        'stop': ['custard'],
        'focus': ['custard', 'group'],
        'move': ['window'],
        'expand': ['window'],
        'contract': ['window'],
        'maximize': ['window'],
        'minimize': ['window'],
        'raise': ['window'],
        'lower': ['window'],
        'relocate': ['window'],
        'attach_to_group': ['window'],
        'detach_from_group': ['window'],
        'close': ['window'],
        'attach': ['group'],
        'detach': ['group']
    }

    optionlist, arguments = getopt.getopt(command_line_arguments,
        '', list(option_target_map.keys()))

    if 'dry' in arguments:
        dry = True

    action = optionlist[0][0][2:]

    if target not in option_target_map[action]:
        error_out("Invalid option for target " + target + ": " + argument)

    if target == 'custard':
        if action == 'help':
            show_help()
        elif action == 'stop':
            send_message("custard halt")
        elif action == 'focus':
            option = arguments[0]; print(option)

            if option not in ['next']:
                error_out("Invalid option for action focus")

            output = "custard focus {0}".format(option)
        elif action == 'configure':
            variable = arguments[0]
            value = arguments[1]

            rgba_vars = [
                'border_focused_color',
                'border_unfocused_color',
                'border_background_color'
            ]

            uint_vars = [
                'border_inner_size',
                'border_outer_size',
                'border_type',
                'grid_rows',
                'grid_columns',
                'grid_gap',
                'grid_margin_top',
                'grid_margin_bottom',
                'grid_margin_left',
                'grid_margin_right',
                'groups'
            ]

            bool_vars = [
                'border_invert_colors',
                'debug_mode'
            ]

            valid_variables = rgba_vars + uint_vars + bool_vars

            if variable not in valid_variables:
                error_out("Invalid variable for --configure: " + variable)

            data = None
            if variable in rgba_vars:
                data = parse_rgba_color(value)
            elif variable in uint_vars:
                data = parse_unsigned_integer(value)
            elif variable in bool_vars:
                data = parse_boolean(value)

            if variable == 'groups':
                if not 1 <= data <= 16:
                    error_out("Invalid number of groups: 1 <= groups <= 16")

            data = str(data)

            output = "{0} configure {1} {2}".format(target, variable, data)
    elif target == 'window':
        if action == 'close':
            output = "window close"
        elif action in ['raise', 'lower']:
            output = "window {0}".format(action)
        elif action in ['move', 'expand', 'contract']:
            value = arguments[0].lower()

            direction_map = {
                'north': 0,
                'northwest': 4,
                'north-west': 4,
                'north_west': 4,
                'northeast': 5,
                'north-east': 5,
                'north_east': 5,
                'south': 1,
                'southwest': 6,
                'south-west': 6,
                'south_west': 6,
                'southeast': 7,
                'south-east': 7,
                'south_east': 7,
                'west': 2,
                'east': 3,
                'all': 8
            }

            direction_number = direction_map[value]
            if action == 'move' and direction_number == 8:
                error_out("Invalid direction specified for action")
            output = "{0} {1} {2}".format(target, action, direction_number)
        elif action in ['attach_to_group', 'detach_from_group']:
            value = arguments[0]
            value = parse_unsigned_integer(value)

            output = "{0} {1} {2}".format(target, action, value)
    elif target == 'group':
        group = arguments[0]
        group = parse_unsigned_integer(group)

        output = "{0} {1} {2}".format(target, action, group)

    send_message(output)
