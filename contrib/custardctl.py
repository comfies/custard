#!/usr/bin/env python3

import sys

FIFO_PATH = "/tmp/custard.fifo"

def usage():
    print("""
    custard v1.3

    custardctl [target] [-action] [argument]

    Possible targets and their respective actions
        custard
            -help
            -stop
            -focus [next|previous|forward|backward]
        window
            -move [up|down|left|right]
            -expand [up|down|left|right]
            -contract [up|down|left|right]
            -maximize
            -minimize
            -close
        workspace
            -focus [n]
            -attach [n]
            -detach [n]

    For more information, see the `custard' man page, or navigate to the GitHub page.

    https://github.com/Sweets/custard
    """)
    exit(1)

def write_command(command):
    fifo = open(FIFO_PATH, "w")
    fifo.write(command)
    fifo.close()

def create_output(alpha, beta, gamma):
    output = list()

    if alpha not in ("custard", "window", "workspace"):
        usage()

    if alpha == "custard":
        output.append(0)

        if beta not in ("stop", "focus", "help"):
            usage()

        if beta == "help":
            usage() # Technically didn't need to test for `help` explicitly

        if beta == "stop":
            output += list((0, 0))
            return output

        if beta == "focus":
            output.append(1)

            if gamma not in ("forward", "next", "backward", "prev"):
                usage()

            if gamma == "forward" or gamma == "next":
                output.append(1)
            if gamma == "backward" or gamma == "prev":
                output.append(0)

            return output

    if alpha == "window":
        output.append(1)

        if beta not in ("move", "expand", "contract", "relocate", "close", "maximize", "minimize"):
            usage()

        if beta == "move":
            output.append(0)
        if beta == "expand":
            output.append(1)
        if beta == "contract":
            output.append(2)
        if beta == "relocate":
            if not gamma.isdigit():
                usage()

            output += list((3, gamma))
            return output

        if beta == "maximize":
            output += list((5, 0))
            return output

        if beta == "minimize":
            output += list((6, 0))
            return output

        if beta == "close":
            output += list((4, 0))
            return output

        if gamma not in ("up", "down", "left", "right"):
            usage()

        if gamma == "up":
            output.append(0)
        if gamma == "down":
            output.append(1)
        if gamma == "left":
            output.append(2)
        if gamma == "right":
            output.append(3)

        return output

    if alpha == "workspace":
        output.append(2)

        if beta not in ("focus", "attach", "detach"):
            usage()

        if not gamma.isdigit():
            usage()

        if beta == "attach":
            output += list((0, gamma))
        if beta == "detach":
            output += list((1, gamma))
        if beta == "focus":
            output += list((2, gamma))

        return output

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

    output = create_output(alpha, beta, gamma)

    if len(output) < 3:
        usage()

    output = "+{}.{}.{};".format(*output)

    write_command(output)

if __name__ == "__main__":
    parse(sys.argv[1:])
