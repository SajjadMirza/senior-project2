#!/bin/bash

export LD_LIBRARY_PATH=lib/bin/all

valgrind --tool=memcheck install/myapp
