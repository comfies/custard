#!/usr/bin/env python3

import sys

FIFO_PATH = "/tmp/custard.fifo"

FIFO_COMMANDS = {
    "window": {
        "close": {
            "output": "close"
        },
        "relocate": {
            "expects": int,
            "output": "go to workspace {0}"
        },
        "move": {
            "expects": "direction",
            "output": "move {0}"
        },
        "expand": {
            "expects": "direction",
            "output": "grow {0}"
        },
        "contract": {
            "expects": "direction",
            "output": "shrink {0}"
        }
    },
    "workspace": {
        "focus": {
            "expects": int,
            "output": "focus {0}"
        },
        "attach": {
            "expects": int,
            "output": "attach {0}"
        },
        "detach": {
            "expects": int,
            "output": "detach {0}"
        }
    },
    "custard": {
        "focus": {
            "expects": "relative_position",
            "output": "cycle focus {0}"
        },
        "stop": {
            "output": "stop"
        }
    }
}

def usage():
    print("thing")
    exit(1)

def write_command(command):
    fifo = open(FIFO_PATH, "w")
    fifo.write(command)
    fifo.close()

def parse(args):
    if len(args) > 3 or len(args) == 0:
        usage()
    elif len(args) == 3:
        alpha, beta, gamma = args
    elif len(args) == 2:

        alpha = "custard"
        beta, gamma = args

        if beta[0] != "-":
            if gamma[0] != "-":
                usage()
            alpha = beta
            beta = gamma
            gamma = ""

    elif len(args) == 1:
        alpha = "custard"
        beta = args[0]
        gamma = ""

    if beta[0] != "-":
        usage()
    else:
        beta = beta[1:]

    alpha, beta, gamma = alpha.lower(), beta.lower(), gamma.lower()

    fifo_command_array = FIFO_COMMANDS.get(alpha, -1)

    if type(fifo_command_array) is int:
        usage()

    fifo_command = fifo_command_array.get(beta, -1)

    if type(fifo_command) is int:
        usage()

    expectation = fifo_command.get('expects', None)

    if type(expectation) is type(None):
        if gamma != "":
            usage()
    elif type(expectation) is int:
        if not gamma.isdigit():
            usage()
    elif type(expectation) is str:
        if expectation == "direction":
            if gamma not in ["up", "down", "left", "right"]:
                usage()

        elif expectation == "relative_position":
            if gamma not in ["next", "prev", "previous", "forward", "backward"]:
                usage()

            if gamma == "next":
                gamma = "forward"
            elif gamma == "prev" or gamma == "previous":
                gamma = "backward"

    output = "{0} {1};".format(alpha,
                              fifo_command.get("output").format(gamma))

    write_command(output)

if __name__ == "__main__":
    parse(sys.argv[1:])
