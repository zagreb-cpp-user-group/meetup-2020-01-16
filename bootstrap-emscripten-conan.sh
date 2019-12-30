#!/bin/bash

# First, setup conan with required remotes
conan remote add dodoent https://api.bintray.com/conan/dodoent/conan || true
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan || true

# Finally, install conan profiles
conan config install conan-config.zip
