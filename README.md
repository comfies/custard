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

## configuration

The command structure for configuring custard is the setting name followed
by the setting value.

```
$ custard -- configure [setting name] [setting value]
```

To apply the new configuration to custard you must issue the reconfigure command
to the socket.

```
$ custard -- reconfigure
```

## configuring the virtual grid

custard creates a virtual grid that windows are moved and sized about. By default, there is only two rows and two columns. The grid may also be offset from the top, left, right, or bottom of the screen, as well as create gaps between windows.

The settings related to the virtual grid, as well as their default values, are listed below.

|Setting Name|Default Value|Accepted Values|
|-|-|-|
|`grid.rows`|2|Any positive integer|
|`grid.columns`|2|Any positive integer|
|`grid.gap`|0|Any positive integer|
|`grid.offset.top`|0|Any positive integer|
|`grid.offset.bottom`|0|Any positive integer|
|`grid.offset.left`|0|Any positive integer|
|`grid.offset.right`|0|Any positive integer|

## window borders

### types and sizes

Windows can have one of four border types, as well as specific sizes.

If the border type is set to single, the border size is equal to the outer border
size.

|Setting|Default Value|Accepted Values|
|-|-|-|
|`border.type`|0|Any positive number less than 4|
|`border.inner.size`|0|Any positive integer|
|`border.outer.size`|0|Any positive integer|

### border colors

Borders can use up to three different colors depending on the border type. You can also swap colors using the `switch` color setting, but a specific set of conditions applies to how this setting works based on the used border type.

|Setting|Default Value|Accepted Values|Conditions|
|-|-|-|-|
|`border.color.focused`|#ffffffff|0x00000000 - 0xffffffff|Only shown when a window is focused|
|`border.color.unfocused`|#ffffffff|0x00000000 - 0xffffffff|Only shown when a window is not focused|
|`border.color.background`|#ffffffff|0x00000000 - 0xffffffff|Only shown when the border type is `2` or `3`|
|`border.color.switch`|`False`|`True` or `False`|Conditions outlined below|

If `border.color.switch` is set to `True`, then custard will swap the focused and unfocused colors. If the border type is set to `1`, this setting will swap the focused color with the background color. If the border type is set to either `2` or `3`, then the setting will swap based on the focused state. If a given window is focused, the background and focused colors are swapped, and if a given window is not focused, the background and unfocused colors are swapped.

### miscellaneous

|Setting|Default Value|Accepted Values|Description|
|-|-|-|-|
|`debug`|`False`|`True` or `False`|Outputs debug data to STDERR|
|`workspaces`|`1`|Any positive integer|The number of workspaces|

# contributing

## commit standards
