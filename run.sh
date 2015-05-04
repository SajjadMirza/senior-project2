#!/bin/bash

FMOD_DIR=lib/bin/fmod
ASSIMP_DIR=lib/bin/assimp

declare -a FILES=(lib/bin/*/*.so)

#echo ${FILES[@]}

LD_PRELOAD=""

for i in ${FILES[@]}
do
#    echo "$i"
    LD_PRELOAD="$LD_PRELOAD $i"
done

#echo "$LD_PRELOAD"

export LD_PRELOAD

install/bin/myapp
