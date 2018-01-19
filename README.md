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
custard >> /tmp/custard.log
# custard various information to STDOUT, useful for debugging.
```

As for a hotkey daemon, you can refer to `examples/sxhkdrc` for usage with sxhkd (which is the hot key daemon that I both use and recommend).

For configuration, see CONFIGURATION, or refer to `examples/.Xresources`.

[To Table of Contents](#table-of-contents)

# Managing Windows

custard manages windows via commands from a named pipe/FIFO. For the purposes of this README, understand that "named pipe" is synonymous with "FIFO", as they will be referred to as FIFOs here.

To send a command, the "formula" is `echo "command" > /tmp/custard.fifo`. Commands, as well as what they do, are specified below.

`move window up` - Moves the focused window up by a single grid unit.

`move window down` - Moves the focused window down by a single grid unit.

`move window left` - Moves the focused window to the left by a single grid unit.

`move window right` - Moves the focused window to the right by a single grid unit.

`grow window up` - Makes the focused window grow by a single grid unit upwards.

`grow window down` - Makes the focused window grow by a single grid unit downwards.

`grow window left` - Makes the focused window grow by a single grid unit to the left.

`grow window right` - Makes the focused window grow by a single grid unit to the right.

`shrink window up` - Makes the focused window shrink by a single grid unit upwards.

`shrink window down` - Makes the focused window shrink by a single grid unit downwards.

`shrink window left` - Makes the focused window shrink by a single grid unit to the left.

`shrink window right` - Makes the focused window shrink by a single grid unit to the right.

`close window` - Closes the focused window.

`go to workspace N` - Goes to the specified workspace, where N is the Nth workspace. custard works with humane positions, i.e., it does NOT use indexes, and thus specifying a value of `0` does not bring you to the first workspace.

`send to workspace N` - Sends the focused window to the Nth workspace.

`attach workspace N` - Attaches the Nth workspace. "Attaching" a workspace is the same as bringing all windows in the workspace to the screen. custard will NOT set any attached workspaces as the active workspace, so EWMH hints will not show any attached workspace as the active workspace.

`detach workspace N` - Detaches the Nth workspace. This hides the workspace specified, if it's not the active workspace.

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
