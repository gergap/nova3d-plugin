# Nova3d Chitubox Plugin

The plugin from Nova3d does not support Linux, like Novamaker.
So I created my own conversion plugin, which runs on Linux and should work also on Windows.

# Status

This is an early version which has the state: "works for me". I tested it with my Nova3d Elfin2 and CHITUBOX V1.9.0.

# Requirements

* CMake
* Ninja
* GCC (for Linux native builds)
* mingw (for Windows cross-compilation)

## For Debian users

```
sudo apt install build-essential
sudo apt install cmake ninja-build
sudo apt install mingw-w64-x86-64-dev (for Windows cross-compilation only)
```

# Building Instructions

If you know CMake this is not much to explain. If not have a look at the *.sh files which are convenience scripts
for easy building.

## Linux

```
./build.sh
```

## Windows

```
./build4Win.sh
```

## All in one Go

```
./build-all.sh
```

