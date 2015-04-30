#!/bin/bash

FMOD_DIR=fmod/api/lowlevel/lib/x86_64
ASSIMP_DIR=assimp

export LD_PRELOAD="$FMOD_DIR/libfmod.so $ASSIMP_DIR/libassimp.so"

install/bin/myapp
