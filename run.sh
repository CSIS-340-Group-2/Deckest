#!/bin/bash
# Only for MSYS2

. ./env.sh

if ! stat ./build >/dev/null; then
	meson build
fi
if ninja -C build/ ; then
	./build/deckest.exe
fi
