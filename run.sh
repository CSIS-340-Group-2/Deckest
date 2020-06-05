#!/bin/bash
# Only for MSYS2

. ./env.sh

clang-format -i src/*.*pp

if ! stat ./build >/dev/null; then
	meson build
fi
if ninja -C build/ ; then
	./build/deckest.exe
fi
