#!/bin/bash
set -e
BASEDIR=`pwd`
BLDDIR=$BASEDIR/bld
DISTDIR=$BASEDIR/dist
BUILD_TYPE=MinSizeRel
GENERATOR="CodeBlocks - Ninja"

if [ "$1" == "clean" ]; then
    rm -rf $BLDDIR
fi

mkdir -p $BLDDIR
cd $BLDDIR
cmake -G"$GENERATOR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$DISTDIR $BASEDIR
ninja
ninja install
ninja package_plugin
