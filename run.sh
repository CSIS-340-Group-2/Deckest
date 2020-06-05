#!/bin/bash
# Only for MSYS2

if ! stat ./build >/dev/null; then
	meson build
fi
if ninja -C build/ ; then
	./build/deckest.exe
fi
