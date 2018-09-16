#!/usr/bin/env python3

import getopt
import re
import socket
import sys

from typing import List

SOCKET_PATH  = '/tmp/custard.sock'
dry = False
option_target_map = dict()

def help() -> None:
    print("help(): stub")
    exit(0)

def send(message: str) -> None:
    global SOCKET_PATH

    message = str(message).lower()
    try:
        unix_socket = socket.socket(socket.AF_UNIX)
        unix_socket.settimeout(1)
        unix_socket.connect(SOCKET_PATH)
        unix_socket.send(message.encode())
        unix_socket.close()
    except (ConnectionRefusedError, FileNotFoundError):
        exit(1)

def hash_string(string: str) -> int:
    assert type(string) is str, "Argument for hash_string() is not a string"
    MAGIC_NUMBER = 5381
    BITMASK = 0xFFFFFFFF

    output = MAGIC_NUMBER

    def recurse(nth: int = 0) -> None:
        nonlocal string, output

        if len(string) - 1 < nth:
            return
        else:
            output = (output * 33) ^ ord(string[nth])
            recurse(nth + 1)

    recurse()

    output = (output >> 0) & BITMASK
    return output

def c_target(*args, **kwargs):
    def decorator(method):
        nonlocal args, kwargs

        target_name = 'custard'
        action_name = method.__name__.lower()

        if kwargs.get('target', False):
            target_name = kwargs.get('target').lower()

        if kwargs.get('command', False):
            action_name = kwargs.get('command').lower()

        def wrapper(*args, **kwargs):
            global dry
            nonlocal method, target_name, action_name
            try:
                method_output = method(*args, **kwargs)
                socket_output = "{0} {1}".format(hash_string(target_name),
                                                 hash_string(action_name))

                if method_output:
                    socket_output += " " + method_output

                if dry:
                    print(socket_output)
                else:
                    send(socket_output)
            except TypeError:
                exit(1) # Error: insufficient arguments


        if option_target_map.get(target_name, False):
            option_target_map.get(target_name).setdefault(action_name, wrapper)
        else:
            option_target_map.setdefault(target_name, {action_name: wrapper})
    return decorator

def call(target: str, action: str, *args, **kwargs) -> None:
    if option_target_map.get(target, False):
        if option_target_map.get(target).get(action, False):
            option_target_map.get(target).get(action)(*args, **kwargs)

class Parse:
    def boolean(user_input: str) -> bool:
        user_input = user_input.lower()
        return (user_input == 'true')

    def unsigned_integer(user_input: str) -> int:
        if user_input.isdigit():
            return int(user_input)
        return 0

    def rgba_color(user_input: str) -> None:
        return


class SocketCommands:
    @c_target(command='stop')
    def halt():
        pass

    @c_target()
    def configure():
        return 'no'

    @c_target(target='window')
    def geometry(name):
        return name

    @c_target(target='window')
    def close():
        pass

if __name__ == '__main__':
    command_line_arguments = sys.argv[1:]
    if len(command_line_arguments) == 0:
        help()

    targets = list(option_target_map.keys())
    actions = list()

    for key, value in option_target_map.items():
        for action in list(value.keys()):
            if action not in actions:
                actions.append(action)

    target = 'custard'

    if command_line_arguments[0] in targets:
        target = command_line_arguments[0]
        command_line_arguments = command_line_arguments[1:]
    else:
        if command_line_arguments[0][0:2] != '--':
            exit(1)

    optionlist, arguments = getopt.getopt(command_line_arguments, '', actions +
                                          ['help', 'dry'])

    options = list()
    for option, value in optionlist:
        if option == '--help':
            help()
        elif option == '--dry':
            dry = True
        else:
            if option[0:2] == '--':
                option = option[2:]
            options.append(option)

    if len(options) > 1:
        exit(1) # Error: more than one action?
    action = options[0]

    if target not in targets:
        exit(1) # Error: non-existent target

    if action not in actions:
        exit(1) # Error: non-existent action (how did you get here?)

    call(target, action, *arguments)
