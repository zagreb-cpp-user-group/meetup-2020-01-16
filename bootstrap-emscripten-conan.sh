#!/bin/bash

hash cmake 2>/dev/null || { echo >&2 "CMake is required but it's not installed. Aborting."; exit 1; }
hash conan 2>/dev/null || { echo >&2 "Conan is required but it's not installed. Aborting."; exit 1; }
hash ninja 2>/dev/null || { echo >&2 "Ninja is required but it's not installed. Aborting."; exit 1; }

# First, setup conan with required remotes
conan remote add dodoent https://api.bintray.com/conan/dodoent/conan || true
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan || true

# Install conan profiles
conan config install conan-config.zip

# Install the emsdk installer
conan install emsdk_installer/1.38.29@bincrafters/stable

# detect conan home location
CONAN_PREFIX=$CONAN_USER_HOME

if [ -z "$CONAN_PREFIX" ]; then
    CONAN_PREFIX=$HOME
fi

# generate activate.sh
EMSDK=$CONAN_PREFIX/.conan/data/emsdk_installer/1.38.29/bincrafters/stable/package/16edc5c715408e005b54d61db41959d8f63a70c2
EMSCRIPTEN=$EMSDK/emscripten/1.38.29
EM_CONFIG=$EMSDK/.emscripten
EM_CACHE=$EMSDK/.emscripten_cache
CMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake

echo "export EMSDK=$EMSDK
export EMSCRIPTEN=$EMSCRIPTEN
export EM_CONFIG=$EM_CONFIG
export EM_CACHE=$EM_CACHE
export PATH=\$PATH:$EMSCRIPTEN

alias ecmake=\"cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE\"" > ./activate.sh

echo "Use 'source ./activate.sh' to activate the Emscripten environment for current shell session and use 'ecmake' instead of 'cmake' for build system generation."
