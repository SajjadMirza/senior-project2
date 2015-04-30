#!/bin/bash

FMOD_DIR=lib/bin/fmod
ASSIMP_DIR=lib/bin/assimp

export LD_PRELOAD="$FMOD_DIR/libfmod.so $ASSIMP_DIR/libassimp.so"

install/bin/myapp
