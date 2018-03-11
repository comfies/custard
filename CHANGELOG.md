# 1.2

## Fixes

## Additions

## Changes

1. Removed sloppy focus in favor of window cycling

# 1.1

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

# 1.0

Initial release.
