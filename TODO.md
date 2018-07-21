# To-do

1. Redefine how focused windows are saved between workspace changes (currently to keep focus, we avoid setting a windows focus to false, but doing so causes a bug with Chromium, and presumably may do so with other programs as well)
2. Proper support for desktop atoms, change desktop requests (ewmh)
3. Cache inner border values in window.cpp, rather than recalculating ever border update, if geometry changes, then recalculate and recache
4. Support for alpha transparency in border colors

# Delayed

1. Add rounded borders (curved windows/shaped X windows), maybe?
2. Multimonitor support (I don't have a second monitor)
