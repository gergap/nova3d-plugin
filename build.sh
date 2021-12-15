#!/bin/bash
set -e
BASEDIR=`pwd`
BLDDIR=$BASEDIR/bld
DISTDIR=$BASEDIR/dist
BUILD_TYPE=MinSizeRel
GENERATOR="CodeBlocks - Ninja"

mkdir -p bld
cd bld
cmake -G"$GENERATOR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$DISTDIR $BASEDIR
ninja
ninja install
cpack .
# rename plugin to what Chitubox expects
cp nova3d_plugin.zip $BASEDIR/nova3d_plugin.CHplugin
echo "Successfully created nova3d_plugin.CHplugin"

