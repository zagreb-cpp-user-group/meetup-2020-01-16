Zagreb C++ Meetup 2020-01-16
========

This repository contains code samples and presentation slides shown during C++ meetup on January 16th, 2020.

[Slides](https://zagreb-cpp-user-group.github.io/meetup-2020-01-16/index.html) are created with markdown and compiled to HTML using [Marpit](https://marpit.marp.app).

# Cloning notes

This repository contains some files on [LFS](https://git-lfs.github.com), so make sure that you have Git LFS support installed prior cloning this repository. Without this, code examples will not work.

# Code example

## Live demo of the example

Live demo of the example is available on [this link](https://zagreb-cpp-user-group.github.io/meetup-2020-01-16/live_demo/index.html)

## Building and running the example

You will need [CMake](https://cmake.org), [Ninja](https://ninja-build.org) and [Conan](https://conan.io) installed on your computer and available in your `PATH`. If running on windows, please use a bash shell.

1. Run the `bootstrap-emscripten-conan.sh` script - it will setup your conan environment for development with emscripten, download the Emscripten conan package and generate an `activate.sh` shell script that will setup your shell for emscripten development (similar to how python virtualenv works).
1. Activate the Emscripten environment

    ```source ./activate.sh```
1. Create and enter an empty build directory

    ```mkdir build && cd build```
1. Initialize the cmake build directory

    ```ecmake -DCMAKE_BUILD_TYPE=Release ..```
1. Build the code

    ```ninja```

    or

    ```cmake --build .```
1. Serve the `bin` directory with simple HTTP server.

    ```python3 -m http.server --directory ./bin 8080```
1. Navigate your browser to `http://localhost:8080` and allow camera access.
1. **Note**: The camera access will work only on `localhost`. If you want other users to have access to the camera, you will need to serve the directory via HTTPS, as most browsers do not allow camera access for [non-secure contexts](https://developer.mozilla.org/en-US/docs/Web/Security/Secure_Contexts).
