#!/bin/bash
# This script cross-compiles Windows binaries using MinGW compiler
set -e
BASEDIR=`pwd`
BLDDIR=$BASEDIR/bldWin
DISTDIR=$BASEDIR/distWin
BUILD_TYPE=MinSizeRel
OPTIONS="-DCMAKE_TOOLCHAIN_FILE=$BASEDIR/toolchains/mingw/toolchain-x86_64-w64-mingw32.cmake"
GENERATOR="CodeBlocks - Ninja"

if [ "$1" == "clean" ]; then
    rm -rf $BLDDIR
fi

mkdir -p $BLDDIR
cd $BLDDIR
cmake -G"$GENERATOR" $OPTIONS -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$DISTDIR $BASEDIR
ninja
ninja install
ninja package_plugin
