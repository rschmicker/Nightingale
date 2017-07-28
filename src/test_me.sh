#!/usr/bin/env bash

make -j $(getconf _NPROCESSORS_ONLN)
./nightgale
make clean
