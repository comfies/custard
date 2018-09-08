# 0.4

## Fixes

1. custardctl raising error for Python < v3.6
2. -Wformat-security, -Wint-conversion, -Wmaybe-uninitialized

## Changes

1. Rewritten in C
2. Groups replace workspaces
3. No longer multithreaded (socket is single threaded)
4. IPC uses hashed strings with switches, instead of testing everything with ifs

# 0.3

## Fixes

1. Segfault fixed when cycling through windows
2. Fixed unused result warnings during compilation
3. Fixed right click/dialog window bug
4. Transient windows are no longer focused on, nor are any windows of the menu types

## Additions

1. Support for `_NET_WM_WINDOW_TYPE_MENU`, `_NET_WM_WINDOW_TYPE_DROPDOWN_MENU`, `_NET_WM_WINDOW_TYPE_POPUP_MENU`, and `_NET_WM_WINDOW_TYPE_DIALOG`
2. Added `custard.desktop` file for display managers

## Changes

1. Removed sloppy focus in favor of window cycling
2. EWMH active window property is now set when focusing on a winow
3. `_NET_WM_STATE` is now set when focusing on a window

# 0.2

## Fixes

1. Segfault fixes in window map handler, border update calls, workspace switching, and the fifo
2. Flickering fixed, albeit very hackily fixed

## Additions

1. Workspaces keep last focused window
2. Configuration checks for invalid rows, columns, or workspaces
3. custardctl
4. Focus cycling feature
5. Proper window killing
6. Support for `_NET_ACTIVE_WINDOW` and `XCB_CONFIGURE_REQUEST`

## Changes

1. Synchronous updates
2. Debug data outputs to STDERR rather than STDOUT

# 0.1

Initial release.
