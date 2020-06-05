#!/bin/bash

# Install the packages needed for DeckEst on MSYS2
# (Ignoring the obvious `pacman -S git` you needed to download me)

pacman -S mingw-w64-x86_64-gtkmm3 mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-pkg-config
