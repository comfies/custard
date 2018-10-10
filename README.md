<div align="center">
<img src="contrib/header.png"><br>
</div>

# table of contents

# description

custard is a manual tiling window manager for the X windowing system.  It
arranges windows in a grid and allows the user to modify which cells of the
grid each window takes up.

custard is based on [2bwm](https://github.com/venam/2bwm),
[swm](https://github.com/dcat/swm), [subtle](https://subtle.subforge.org/),
and [howm](https://github.com/HarveyHunt/howm).

# installing

Installing is done on a per-user basis (except for the man pages).
Installation will provide the `custard` and `custardctl` commands that are
necessary for the window manager to function.

## dependencies

|OS          |Packages                                                |
|------------|--------------------------------------------------------|
|Arch Linux  |`libxcb xcb-util xcb-util-wm`                           |
|Debian Linux|`xcb libxcb-ewmh-dev libxcb-icccm4 libxcb-util0-dev`    |
|Gentoo Linux|`x11-libs/libxcb x11-libs/xcb-util x11-libs/xcb-util-wm`|
|Void Linux  |`libxcb-devel xcb-util-devel xcb-util-wm-devel`         |
|OpenBSD     |`/usr/ports/x11/xcb`                                    |

If the dependencies for your operating system are not listed yet, feel free to
contribute them to the list.

If your operating system does not provide them as packages you can collect
them from freedesktop via git and build them from source.

```
$ git clone git://anongit.freedesktop.org/xcb/libxcb
$ git clone git://anongit.freedesktop.org/xcb/util
$ git clone git://anongit.freedesktop.org/xcb/util-wm
```

## automatic install

custard comes packaged with a script that automagically compiles and installs
custard, as well as copying the configuration files to
`$XDG_CONFIG_HOME/custard/`. The install script assumes that all dependencies
have been met and will behave unexpectedly if they are not.

### automatic configuration

## manual install

Doing the install manually will *not* automatically set up sxhkd or
custard configuration files.

```
$ git clone https://github.com/Sweets/custard
$ cd custard
# make install
# sudo make install
# make clean
```

## post install

# usage

custard works by having a server and a client interact with eachother over
[the socket](#the-socket), which is a UNIX socket, by default located at
`/tmp/custard.sock`. The client sends messages over the socket which are
read by the server and followed through with. Currently, the default method
of facilitating this is `custardctl` which adds another layer of abstraction
whereby arguments passed to `custardctl` are translated into the actual
commands sent to the server.

## overview

The socket is located at `/tmp/custard.sock` by default. To interface with
custard, a string must be sent to the socket, structure as
`<target>;<action>;<arguments...>`. The maximum accepted buffer size is 1024
bytes, and there can be at most 8 arguments in a command sent to the socket.

The `target` and `action` are both strings hashed using the djb2 hashing
algorithm. Arguments are not hashed.

## socket commands

## using custardctl to manage windows

# configuration

# contributing

## commit standards
