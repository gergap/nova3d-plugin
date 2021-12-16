#!/bin/bash
# Simple batch job for generating all platforms
set -e

./build.sh clean
./build4Win.sh clean
