# To-do

1. Remove FIFO in favor of an IPC
2. Make custard configurable via custardctl, after the IPC implementation
3. Redefine how focused windows are saved between workspace changes (currently to keep focus, we avoid setting a windows focus to false, but doing so causes a bug with Chromium, and presumably may do so with other programs as well)
4. Proper support for desktop atoms, change desktop requests (ewmh)
5. Cache inner border values in window.cpp, rather than recalculating ever border update, if geometry changes, then recalculate and recache
6. Support for alpha transparency in border colors
7. Condense focus_north, focus_south, focus_west, focus_east into a single function, focus_cardinal(short unsigned int direction), direction integers are 0 for north, 1 south, 2 west, 3 east

# Delayed

1. Add rounded borders (curved windows/shaped X windows), maybe?
1. Multimonitor support (I don't have a second monitor)
