<div align="center">
<img src="contrib/header.png"><br>
</div>

* [description](#description)
* [installing](#installing)
	- [dependencies](#dependencies)
	- [automatic install](#automatic-install)
		- [automatic configuration](#automatic-configuration)
	- [manual install](#manual-install)
	- [post install](#post-install)
* [usage](#usage)
	- [overview](#overview)
	- [socket commands](#custard-commands)
	- [using custardctl](#using-custardctl) (WIP)
* [contributing](#contributing)
	- [style standards](#style-standards)
	- [commit standards](#commit-standards)

# description

custard is a manual tiling window manager for the X windowing system.  It
arranges windows in a grid and allows the user to modify which cells of the
grid each window takes up.

custard is based on [2bwm](https://github.com/venam/2bwm),
[swm](https://github.com/dcat/swm), [subtle](https://subtle.subforge.org/),
and [howm](https://github.com/HarveyHunt/howm). Some parts of the code base
are directly based on code within the projects listed, and credit is due to
the original authors where their code is used.

# installing

Installing is done on a per-user basis (except for the man pages).
Installation will provide the `custard` and `custardctl` commands that are
necessary for the system to function.

## dependencies

A soft depend for custard `sxhkd`. This is not entirely needed by custard
to compile, but it is useful for keybinding. You can theoretically use any
keybiding tool to control window movements, however `sxhkd` is by far the
most simple to configure that also functions well.

|OS          |Packages                                                |
|------------|--------------------------------------------------------|
|Arch Linux  |`libxcb xcb-util xcb-util-wm`                           |
|Debian Linux|`xcb libxcb-ewmh-dev libxcb-icccm4 libxcb-util0-dev`    |
|Gentoo Linux|`x11-libs/libxcb x11-libs/xcb-util x11-libs/xcb-util-wm`|
|Void Linux  |`libxcb-devel xcb-util-devel xcb-util-wm-devel`         |
|OpenBSD     |`/usr/ports/x11/xcb`                                    |

If the dependencie(s) for your OS are not listed yet, feel free to
contribute them to the list.

If you're distro does not provide them as packages you can collect their
source for from freedesktop via git and build them from source.

```
$ git clone git://anongit.freedesktop.org/xcb/libxcb
$ git clone git://anongit.freedesktop.org/xcb/util
$ git clone git://anongit.freedesktop.org/xcb/util-wm
```

## automatic install

custard comes packaged with a script that automagically compiles and installs
custard on a per user basis, as well as copying the configuration files to
`$XDG_CONFIG_HOME/custard/`. The install script assumes that all dependencies
have been met and will behave unexpectedly if they are not.

### automatic configuration

The automatic configuration for custard provides an `sxhkdrc` for
keybindings and `config.sh` which is a shell script that calls a set of
`custardctl` commands used to configure how custard looks and works. These
can be found in the `example/` directory and used with a [manual
installation](#manual-install).

## manual install

Doing the install manually will *not* automatically set up the sxhkd or
custard configurations.

```
$ git clone https://github.com/sweets/custard
$ cd custard
# make install clean
```

## post install

After installing custard either manually or automatically set it in your
DM's config or append, `exec custard` to the end of your `.xinitrc` file.

# usage

Custard works by having a server and a client interact with eachother over
[the socket](#the-socket), which is a UNIX socket, by default located at
`/tmp/custard.sock`. The client sends messages over the socket which are
read by the server and followed through with. Currently, the default method
of facilitating this is `custardctl` which adds another layer of abstraction
whereby arguments passed to `custardctl` are translated into the actual
commands sent to the server.

## overview

The socket, by default located at `/tmp/custard.sock` is what interfaces a
custard client to the core custard server. Messages to the socket are
structured as `<category> <command> (p1) (p2)`, where `category` and
`command` are required but `p1` and `p2` are not, depending on the command.

## socket commands

|action                                |command                              |
|--------------------------------------|-------------------------------------|
|[configure custard](#configuration)   |`custard configure <setting> <value>`|
|halt                                  |`custard halt`                       |
|move/expand/contract window up        |`window [move/expand/contract] 0`    |
|move/expand/contract window down      |`window [move/expand/contract] 1`    |
|move/expand/contract window left      |`window [move/expand/contract] 2`    |
|move/expand/contract window right     |`window [move/expand/contract] 3`    |
|move/expand/contract window up+left   |`window [move/expand/contract] 4`    |
|move/expand/contract window up+right  |`window [move/expand/contract] 5`    |
|move/expand/contract window down+left |`window [move/expand/contract] 6`    |
|move/expand/contract window down+right|`window [move/expand/contract] 7`    |
|expand/contract window all directions |`window [expand/contract] 8`         |
|raise window to top of stack          |`window raise`                       |
|lower window to bottom of stack       |`window lower`                       |
|attach window to group `N`            |`window attach_to_group N`           |
|detach window from group `N`          |`window detach_from_group N`         |
|close window                          |`window close`                       |
|attach group `N` to the screen        |`group attach N`                     |
|detach group `N` from the screen      |`group detach N`                     |
|focus group `N`                       |`group focus N`                      |

## configuration commands

## using custardctl to manage windows

# configuration

# contributing

*note: the following is extremely outdated and needs to be reworked for the
modern version(s) of custard*

To keep the coding style consistent throughout the core codebase, there is a
set of rules that contributors must follow in their contributions

1. curly braces must have their own dedicated lines
2. data-types must be on the same line as the method name and parameters
3. method calls with 3 or more paramaters, or method calls with names
reaching past 80 characters should dedicate a line to each paramater
4. if statements must always wrap their code blocks with curly-braces, even
if it's only one line
5. avoid break statements
6. external dependencies should be exclusively in `main.c` 

## commit standards
Commits should represent at most three logical changes to the codebase, and
be prefixed with `Change`, `Fix`, `Add`, or `Remove`. Commit message should
be as descriptive as possible.
