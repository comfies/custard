<div align="center">
<img src="contrib/header.png"><br>
</div>

# table of contents

# description

custard is a manual tiling window manager for the X windowing system. Users
configure a grid size and create geometries that windows are sized after.

custard is based on [2bwm](https://github.com/venam/2bwm),
[swm](https://github.com/dcat/swm), [subtle](https://subtle.subforge.org/),
and [howm](https://github.com/HarveyHunt/howm).

# installing

TODO: write this

## dependencies

|OS          |Packages                                                |
|------------|--------------------------------------------------------|
|Arch Linux  |`libxcb xcb-util xcb-util-wm libconfig`                 |
|Debian Linux|`xcb libxcb-ewmh-dev libxcb-icccm4 libxcb-util0-dev libconfig-dev libxcb-icccm4-dev`|
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
$ sudo make install
$ make clean
```

## post install

# usage

The `custard` binary is used as both the window manager and the window manager
controller based on the arguments passed in the commandline.

The window manager creates a UNIX socket located at `/tmp/custard.sock`, and
running the `custard` command with two hyphens surrounded by spaces prefixing
socket command.

## overview

## socket commands

# configuration

The command structure for configuring custard is the setting name followed
by the setting value.

```
$ custard -- configure [setting name] [setting value]
```

|Setting|Accepted Arguments|Description|
|-|-|-|
|`debug`|`True` or `False`|Enables or disables debug mode|
|`grid.columns`|Any positive integer|Sets the amount of columns for the grid|
|`grid.rows`|Any positive integer|Sets the amount of rows for the grid|
|`grid.gap`|Any positive integer|Sets the gap size in pixels|
|`grid.offset.top`|Any positive integer|Amount of pixels to offset the grid from the top of the screen|
|`grid.offset.bottom`|Any positive integer|Amount of pixels to offset the grid from the bottom of the screen|
|`grid.offset.left`|Any positive integer|Amount of pixels to offset the grid from the left of the screen|
|`grid.offset.right`|Any positive integer|Amount of pixels to offset the grid from the right of the screen|
|`border.type`|Either `0`, `1`, `2`, or `3`|Sets the border type for windows|
|`border.inner.size`|Any positive integer|Sets the inner size of the border|
|`border.outer.size`|Any positive integer|Sets the outer size of the border|
|`border.color.focused`|#RRGGBBAA|Sets the focused color for borders|
|`border.color.unfocused`|#RRGGBBAA|Sets the unfocused color for borders|
|`border.color.background`|#RRGGBBAA|Sets the background color for borders|
|`border.color.switch`|`True` or `False`|Switches the focused and unfocused or background colors of borders depending on the border.type setting|
|`workspaces`|Any positive integer|Sets the amount of workspaces|

Too apply the new configuration to custard you must send the reconfigure command.

```
$ custard -- reconfigure
```

# contributing

## commit standards
