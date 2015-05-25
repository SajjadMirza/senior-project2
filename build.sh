#!/bin/bash

BUILD_DIR=clibuild

mkdir $BUILD_DIR
cd $BUILD_DIR

cmake .. 
#make
make install
