# custard
custard window manager

custard (stylized in all lowercase letters) is a stacking/floating window manager for X.Org. It arranges windows on a grid, i.e. there is a virtual grid on the screen, windows are positioned by X and Y coordinates, as well as sized over X and Y coordinates.

custard is based on subtle, 2bwm, swm, and howm. Some of the codebase was based off of code from the various projects listed, and as such, thanks, and credit where due to the creators of said projects.

# Table of Contents

1. [Installation](#installation)
2. [Usage](#usage)
3. [Managing Windows](#managing-windows)
4. [Configuration](#configuration)

# Installation

custard depends on XCB to run. Specifically, libxcb, xcb-util, xcb-util-wm, and xcb-util-xrm. These can be found at https://cgit.freedesktop.org/xcb, with the exception of xcb-util-xrm. **It is recommended that you check your distros repos before cloning from these git repos, especially in the interest of stability.**

```
$ git clone git://anongit.freedesktop.org/xcb/libxcb
$ git clone git://anongit.freedesktop.org/xcb/util
$ git clone git://anongit.freedesktop.org/xcb/util-wm
$ git clone git://github.com/Airblader/xcb-util-xrm
```

To install custard after the above dependencies have been met, run the following.

```
$ git clone https://github.com/Sweets/custard.git
$ cd custard
$ make
$ sudo make install
```

[To Table of Contents](#table-of-contents)

# Usage

To use custard, simply execute `custard` in xinit. Do note that you will be launched into the window manager without any key bindings or way to bring up a terminal. As long as you can echo out commands to the fifo, you are able to manage windows.

It is recommended that first time users put the following in their xinit.

```
urxvt &
#or your terminal here
echo "" > /tmp/custard.log
custard 2>> /tmp/custard.log
# custard outputs information to STDERR, useful for debugging.
```

As for a hotkey daemon, you can refer to `examples/sxhkdrc` for usage with sxhkd (which is the hot key daemon that I both use and recommend).

For configuration, see CONFIGURATION, or refer to `examples/.Xresources`.

[To Table of Contents](#table-of-contents)

# Managing Windows

Note: as of the most recent custard git commits, these commands will not work when sent to the fifo. Documentation will not be updated for this, as a tool is being written to supplement these. It is recommended that you do not update, unless you feel like reading the source for commands. (relevant file is src/fifo.cpp)

custard manages windows via commands from a named pipe/FIFO. For the purposes of this README, understand that "named pipe" is synonymous with "FIFO", as they will be referred to as FIFOs here.

A tool is provided by custard that makes things easier on end-users for managing windows. The command and its usage, as well as its shell equivalent command is listed.

`custardctl -stop` - Stops the window manager.

`echo "custard stop;" > /tmp/custard.fifo`



`custardctl -focus [next|prev|previous|forward|backward]` - Cycles the focus forward or backward.

`echo "custard cycle focus [forward|backward];" > /tmp/custard.fifo`



`custardctl window -close` - Closes the currently focused window.

`echo "window close;" > /tmp/custard.fifo`



`custardctl window -relocate N` - Moves the currently focused window to the Nth workspace.

`echo "window go to workspace N;" > /tmp/custard.fifo`



`custardctl window -move [up|down|left|right]` - Moves the currently focused window up, down, left, or right.

`echo "window move [up|down|left|right];" > /tmp/custard.fifo`



`custardctl window -expand [up|down|left|right]` - Makes the currently focused window grow in the specified direction.

`echo "window grow [up|down|left|right];" > /tmp/custard.fifo`



`custardctl window -contract [up|down|left|right]` - Makes the currently focused window shrink in the specified direction.

`echo "window shrink [up|down|left|right];" > /tmp/custard.fifo`



`custardctl workspace -focus N` - Focuses on the Nth workspace.

`echo "workspace focus N;" > /tmp/custard.fifo`



`custardctl workspace -attach N` - Attaches the Nth workspace.

`echo "workspace attach N;" > /tmp/custard.fifo`



`custardctl workspace -detach N` - Detaches the Nth workspace, if attached.

`echo "workspace detach N;" > /tmp/custard.fifo`

[To Table of Contents](#table-of-contents)

# Configuration

custard is configured via the X resource manager, i.e., the Xresources file.

`custard.rows` - How many rows the grid is configured with. Default value is `2`.

`custard.columns` - How many columns the grid is configured with. Default value is `3`.

`custard.border_type` - The border type. There are three valid border types, `Single`, `Double`, `Triple`. There can only be one of these values set. If anything other than these values or their lowercase equivalents are specified, custard assumes that there should be no border. Default value is `None`.

`custard.border_size` - How many pixels a single border should have. If the border type is `Double` or `Triple`, custard will multiple this value by 2 or 3, respectively. Specifying a border size of 0, or leaving this value unspecified (which will cause custard to default), is equivalent to using border type `None`. Default value is `0`.

`custard.focused_color` - The border color for focused windows. Colors are specified in hexadecimal notation, i.e., #RRGGBB (e.g. #000000 is the same as black). Default value is `#FFFFFF`.

`custard.unfocused_color` - The border color for unfocused windows. Colors are specified in hexadecimal notation. Default value is `#676767`.

`custard.background_color` - The background color for the borders of windows. If the border type is `Single`, this value is ignored. Colors are specified in hexadecimal notation. Default value is `#000000`.

`custard.invert_colors` - Inverts the border colors. If border type is `Single`, custard will use the background color for the border color. If the border type is `Double` or `Triple`, custard will flip colors accordingly, i.e., focused windows will have the background color and focused color flipped,  and unfocused windows will have the background and unfocused color flipped.

`custard.margin` - How many pixels each window should have in-between them, i.e. the gap size. Default value is `0`.

`custard.margin_top` - How many pixels to offset the grid from the top of the screen by. Default value is `0`.

`custard.margin_bottom` - How many pixels to offset the grid from the bottom of the screen by. Default value is `0`.

`custard.margin_left` - How many pixels to offset the grid from the left of the screen by. Default value is `0`.

`custard.margin_right` - How many pixels to offset the grid from the right of the screen by. Default value is `0`.

`custard.workspaces` - How many workspaces custard should create during initialization. Default value is `2`.

[To Table of Contents](#table-of-contents)
