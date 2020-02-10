<div align="center">
<img src="contrib/header.png"><br>
</div>

# table of contents

* **[installation](#installation)**
    * **[dependencies](#dependencies)**
    * **[automatic install](#automatic-install)**
    * **[manual install](#manual-install)**
* **[usage](#usage)**
    * **[commandline arguments](#commandline-arguments)**
    * **[using custard with xinit](#using-custard-with-xinit)**
    * **[using custard with a display manager](#using-custard-with-a-display-manager)**
* **[configuration](#configuration)**
    * **[configuring the virtual grid](#configuring-the-virtual-grid)**
    * **[window borders](#window-borders)**
        * **[types and sizes](#types-and-sizes)**
        * **[border colors](#border-colors)**
        * **[miscellaneous](#miscellaneous)**

# description

custard is a manual tiling window manager for the X windowing system. Users
configure a grid size and create geometries that windows are sized after.

custard is based on [2bwm](https://github.com/venam/2bwm),
[swm](https://github.com/dcat/swm), [subtle](https://subtle.subforge.org/),
and [howm](https://github.com/HarveyHunt/howm).

# installation

## dependencies

|OS          |Packages                                                |
|------------|--------------------------------------------------------|
|Arch Linux  |`libxcb xcb-util xcb-util-wm`                           |
|Debian Linux|`xcb libxcb-ewmh-dev libxcb-icccm4-dev libxcb-util0-dev libxcb-randr0-dev libpcre3-dev`|
|Gentoo Linux|`x11-libs/libxcb x11-libs/xcb-util x11-libs/xcb-util-wm`|
|Void Linux  |`libxcb-devel xcb-util-devel xcb-util-wm-devel pcre-devel`|
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

###### [return to table of contents](#table-of-contents)

## Installation

```
$ git clone https://github.com/Sweets/custard
$ cd custard
$ make
$ sudo make install
$ make clean
```

###### [return to table of contents](#table-of-contents)

# usage

## commandline arguments

|Argument|Function|
|-|-|
|`--debug`|Enables the debug mode from the command-line|
|`--rc`|Sets the rc file that will be ran when custard starts. The next argument must be a file that is both readable and executable.|

If the first argument of the commandline after the `custard` binary is a single hyphen
(`-`) then custard will start as a controller and not a window manager.

###### [return to table of contents](#table-of-contents)

## using custard with xinit

###### [return to table of contents](#table-of-contents)

## using custard with a display manager

TODO: this

###### [return to table of contents](#table-of-contents)

---

# configuration

```
$ custard - configure ([setting name] [setting value])...
```

The configure command allows you  to change variables that are used by the
window manager as a whole.

Any amount of setting name and value pairs can be provided to the controller,
and all of the settings will be changed.

|Setting name|Default value|Accepted inputs|Behavior|
|-|-|-|-|
|`grid.rows`|`2`|Any positive integer|Sets the default amount of rows for a grid|
|`grid.columns`|`3`|Any positive integer|Sets the default amount of columns for a grid|
|`grid.margins`|`0`|Any positive integer|Sets the default grid gap size|
|`grid.margin.top`|`0`|Any position integer|Sets the default grid offset from the top of the screen|
|`grid.margin.bottom`|`0`|Any position integer|Sets the default grid offset from the bottom of the screen|
|`grid.margin.left`|`0`|Any position integer|Sets the default grid offset from the left of the screen|
|`grid.margin.right`|`0`|Any position integer|Sets the default grid offset from the right of the screen|
|`borders`|`0`|Any positive integer|Sets the default amount of borders for a window. Currently a maximum of three borders is supported.|
|`border.size.inner`|`0`|Any positive integer|Sets the default inner-border size for a window. Unused for single-border windows|
|`border.size.outer`|`0`|Any positive integer|Sets the default inner-border size for a window|
|`border.color.focused`|`#FFFFFFFF`|Any hexadecimal color, with or without an alpha channel|Sets the default focused color for a window border|
|`border.color.unfocused`|`#676767FF`|Any hexadecimal color, with or without an alpha channel|Sets the default unfocused color for a window border|
|`border.color.background`|`#000000FF`|Any hexadecimal color, with or without an alpha channel|Sets the default background color for a window. Also used as a border background color for two or more borders|
|`border.colors.flipped`|`False`|Any boolean (`True` or `False`)|Flips border colors between the focus-state color and background color|
|`workspaces`|`1`|Any positive integer|Sets the amount of available workspaces. Workspaces are available per monitor|

### Example usage

```
$ custard - configure \
    grid.rows               2 \
    grid.columns            3 \
    grid.margins            0 \
    grid.margin.top         0 \
    grid.margin.bottom      0 \
    grid.margin.left        0 \
    grid.margin.right       0 \
    borders                 0 \
    border.size.outer       0 \
    border.size.inner       0 \
    border.color.focused    '#FFFFFFFF' \
    border.color.unfocused  '#676767FF' \
    border.color.background '#000000FF'
```

###### [return to table of contents](#table-of-contents)

# configuring the virtual grid

custard uses a virtual grid to size and position windows on the screen.
There are global settings that the window manager uses to configure a grid,
but each grid is per monitor output, and as such, each monitor can have their
own grid settings.

###### [return to table of contents](#table-of-contents)

---
