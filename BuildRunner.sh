#!/bin/bash

CONFIG=${1:-Debug}
if [[ "$CONFIG" != "Debug" && "$CONFIG" != "Release" ]]; then
    echo "Usage: $0 [Debug|Release]"
    exit 1
fi

if [ ! -d build ]; then
    mkdir build
fi

if [ ! -d "build/$CONFIG" ]; then
    mkdir "build/$CONFIG"
fi

cd "build/$CONFIG"
cmake ../.. -G Ninja -DCMAKE_TOOLCHAIN_FILE=../../cmake/toolchain-arm-gcc.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=$CONFIG
ninja

cd ../..

if [ ! -d "buildTests/$CONFIG" ]; then
    mkdir "buildTests/$CONFIG"
fi

cd "buildTests/$CONFIG"
cmake ../../UnitTests -G Ninja -DCMAKE_BUILD_TYPE=$CONFIG
ninja

cd ../..

