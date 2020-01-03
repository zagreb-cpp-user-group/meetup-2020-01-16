---
marp: true
theme: gaia
backgroundColor: black
color: white
paginate: false
style: |
    a {
        color: white
    }
    h1, h2, h3, h4, h5, h6 {
        color: #48b2e8
    }
    code, code[class*='language'] {
        background: #222
    }
    section.lead h2 {
        color: #48b2e8;
        text-align: left;
        font-size: xx-large;

        position: relative;
        top: 6em
    }
    section.lead h3 {
        color: #48b2e8;
        text-align: right;
        font-size: xx-large;

        position: relative;
        top: 2em
    }
    table {
        width:100%;
    }
---

<!-- _class: lead -->

# Porting your C++ code to the web with Emscripten

## Nenad Mikša,
## Head of TSI @ Microblink

### @dodo at cpplang.slack.com
### meetup@microblink.com

---

# Emscripten

> a toolchain for compiling to asm.js and WebAssembly, built using LLVM, that lets you run C and C++ on the web at near-native speed without plugins.

- compiler based on LLVM (clang)
- SDK providing common libraries
    - file system IO, OpenGL, SDL, threading, networking, ...

---

# Emscripten compiler

- frontend
    - a python script
- backend
    - a c++ compiler
    - two available backends:
        - fastcomp
        - upstream

---

## Fastcomp backend

- compilation steps:
    - C++ ➞ Bitcode
- linking steps:
    - Bitcode ➞ Javascript ➞ WebAssembly

---

## Fastcomp build steps

- C++ ➞ Bitcode
    - based on clang 6 (partial c++17 support)
- Bitcode ➞ Javascript
    - custom LLVM backend (_fastcomp_)
    - not available in mainline LLVM
- Javascript ➞ WebAssembly
    - _asm2wasm_ - part of [binaryen project](https://github.com/WebAssembly/binaryen)

---

### Fastcomp backend - advantages and disadvantages

- slow linking phase
    - effectively LTO always enabled
- suboptimal performance and code size
    - due to Javascript in compilation step
- stable
- default backend until very recently (December 2019)

---

## Upstream backend

- compilation steps:
    - C++ ➞ WebAssembly
- linking steps:
    - concatenation of WebAssembly object files
    - dead code removal
    - optionally _wasm2js_

---

## Upstream build steps

- C++ ➞ WebAssembly
    - based on upstream LLVM project (currently clang 10)
    - all latest C++ features
- WebAssembly ➞ Javascript
    - optional - only to support old browsers

---

### Upstream backend - advantages and disadvantages

- fast linking
- supports latest c++ features
- direct C++ ➞ WebAssembly compilation
- part of mainline LLVM
- default backend as of emscripten v1.39.0
- still too unstable for production use
    - common compiler crashes (ICEs)
- LTO completely broken

---

# WebAssembly

> a binary instruction format for a stack-based virtual machine. Wasm is designed as a portable target for compilation of high-level languages like C/C++/Rust, enabling deployment on the web for client and server applications.

---

## Differences to Javascript

| WebAssembly                        | Javascript                          |
| :-                                 | :-                                  |
| binary                             | textual                             |
| low-level (difficult to decompile) | high-level (difficult to obfuscate) |
| AOT compilation                    | JIT                                 |
| custom memory                      | garbage-collected heap              |

---

## Current limitations of WebAssembly

- no support for threads
- no support for SIMD
- no access to system resources (files, network sockets, ...)
- behaves as an embedded system
- needs initialization from Javascript

---

## Future of WebAssembly

- support for [threads](https://github.com/WebAssembly/proposals/issues/14)
- support for [SIMD](https://github.com/WebAssembly/proposals/issues/1)
- [WebAssembly System Interface](https://hacks.mozilla.org/2019/03/standardizing-wasi-a-webassembly-system-interface/)
    - will provide access to system resources without the need for JS glue code
- avoiding JS initialization code ([ES6 module integration](https://github.com/WebAssembly/proposals/issues/12))
- [full list of planned features](https://webassembly.org/docs/future-features/)

---

# Emscripten SDK

- provides missing WebAssembly features in terms of Javascript glue code
    - [threads using WebWorkers and SharedArrayBuffer](https://emscripten.org/docs/porting/pthreads.html)
    - [file system support](https://emscripten.org/docs/porting/files/index.html)
    - [OpenAL implementation using Audio API](https://emscripten.org/docs/porting/Audio.html)
    - [EGL and OpenGL implementation using WebGL](https://emscripten.org/docs/porting/multimedia_and_graphics/index.html)
- [supports mixing C++ and Javascript code](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html#interacting-with-code)
- [provides a simple facility for exporting C++ functions to Javascript](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html)
- automatically generates JS initialization code

---

<!-- _class: lead -->

# Thank you
