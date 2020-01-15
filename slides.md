---
marp: true
theme: microblink
paginate: false
---

<!-- _class: title -->

# Porting your C++ code to the web with Emscripten

Nenad Mikša, Head of TSI @ Microblink

@dodo at cpplang.slack.com
meetup@microblink.com

---

# Emscripten

From [emscripten.org](https://emscripten.org):
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

### Fastcomp build steps

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

### Upstream build steps

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

From [webassembly.org](https://webassembly.org):
> a binary instruction format for a stack-based virtual machine. Wasm is designed as a portable target for compilation of high-level languages like C/C++/Rust, enabling deployment on the web for client and server applications.

---
<!-- class: table -->
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

---

# Emscripten SDK (continued)

- [supports mixing C++ and Javascript code](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html#interacting-with-code)
- [provides a simple facility for exporting C++ functions to Javascript](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html)
- automatically generates JS initialization code
- provides CMake toolchain file for easy porting of CMake-based projects

---

## POSIX threads support

- simply compile and link your code with `-s USE_PTHREADS=1` and use the usual `pthread_*` set of functions or `std::thread`
- features require support for [WebWorkers](https://developer.mozilla.org/en-US/docs/Web/API/Worker) and [SharedArrayBuffer](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer)
    - SharedArrayBuffer disabled in most browsers due to [Spectre and Meltdown set of bugs](https://meltdownattack.com)
    - however, it's available in Google Chrome for Desktop v67 and above
- if building without thread support, `pthread_*` functions are still available, but calling them will crash your code

---

## File system support

- implemented with standard `fopen` and friends
- works with `std::fstream` and other abstractions over `fopen` and friends
- all required files need to be packaged at link time using `--preload-file` linker option
    - `--preload-file real/path@/virtual/path`
    - all packaged files are packed into a single `.data` file
    - automatically generated JS code automatically downloads the `.data` file and initializes the virtual file system during runtime initialization

---

## File writing support

- multiple backends for standard functions:
    - MEMFS
        - in-memory, all writes lost after page reload
    - NODEFS
        - wraps NodeJS filesystem functions
    - IDBFS
        - uses browser's IndexedDB for storing data
    - WORKERFS
        - read-only access to `File` and `Blob` objects inside a web worker

---

## File system backends

- by default, MEMFS is used (and is always mounted at `/`)
- Javascript code can initialize other backends for other mount points
    - this is currently not possible from C++ 😞
- Emscripten also has its own [Asynchronous File System API](https://emscripten.org/docs/api_reference/emscripten.h.html#emscripten-h-asynchronous-file-system-api)
    - API to fetch files from the web asynchronously
    - a separate API, not a backend for `fopen` and friends

---

## EGL and OpenGL ES

- imlemented with JS glue code forwarding GL calls to WebGL
- [EGL](https://emscripten.org/docs/porting/multimedia_and_graphics/EGL-Support-in-Emscripten.html) used for creating WebGL context
    - identical procedure as in Android NDK
- [WebGL is subset of OpenGL ES 2.0/3.0](https://emscripten.org/docs/porting/multimedia_and_graphics/OpenGL-support.html)
    - if only compatible features are used, mapping is 1:1
    - Emscripten also supports emulating OpenGL ES features not available in WebGL
        - however, this comes with some performance cost
- both WebGL 1 and WebGL 2 are supported

---

## Mixing C++ and Javascript code

```c++
#include <emscripten.h>

EM_JS(void, call_alert, (), {
  alert('hello world!');
  throw 'all done';
});

int main() {
  call_alert();
  return 0;
}
```

---

### Inline mixing of code

```c++
#include <emscripten.h>

int main() {
  EM_ASM(
    alert('hello world!');
    throw 'all done';
  );
  return 0;
}
```

---

## Passing parameters from C++ to JS

```c++
template< typename Outcome >
auto succeedOrDie( Outcome && outcome ) {
    if ( outcome.has_value() ) {
        return std::forward< Outcome >( outcome ).value();
    } else {
        EM_ASM_( {
            throw new Error( UTF8ToString( $0 ) );
        }, outcome.error().data() );
    }
}
```

---

## Exporting C++ functions to Javascript

```c++
#include <math.h>
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
int int_sqrt(int x) {
  return sqrt(x);
}
```

To call from Javascript, use `cwrap` or `ccall`:

```js
var int_sqrt = Module.cwrap('int_sqrt', 'number', ['number']);
var result = int_sqrt(12);
```

---

## Exporting C++ functions to Javascript (2)

Calling with `ccall`:

```js
// Call C from JavaScript
var result = Module.ccall('int_sqrt', // name of C function
  'number', // return type
  ['number'], // argument types
  [28]); // arguments
```

Direct call (works only for primitive types):

```js
var result = Module._int_sqrt(28);
```

---

## Exporting C++ classes and value objects

- very difficult and pointless to do that manually
- two possible solutions:
    - [Embind](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html)
    - [WebIDL Binder](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/WebIDL-Binder.html)

---

# Embind

- requires C++14
- entire JS interface defined in C++ code
- bindings are specified in `EMSCRIPTEN_BINDINGS` blocks

---

## Embind class binding example

```c++
class EmscriptenFaceDetector
{
public:
    EmscriptenFaceDetector();
    EmscriptenFaceDetection detectFaces( emscripten::val const & jsImageData );

private:
    FaceDetector detector_;
    cv::Mat rgbaImage_;
};
```

---

## Embind bindings code

```c++
using namespace emscripten;
EMSCRIPTEN_BINDINGS( FaceDetector )
{
    class_< EmscriptenFaceDetector >( "FaceDetector" )
        .constructor()
        .function( "detectFaces", &EmscriptenFaceDetector::detectFaces );
}
```

---

## JS class usage

```js
var faceDetector = new Module.FaceDetector();
var detectionResult = faceDetector.detectFaces( image );

// free the memory
faceDetector.delete();
```

- **NOTE**
    - JS does not support finalizers
    - JS garbage collector will cleanup only the JS proxy object
    - `delete` must be called to prevent leaks on the emscripten heap

---

## Embind value object example

```c++
struct EmscriptenFaceDetection
{
    bool detected = false;
    int  x        = 0;
    int  y        = 0;
    int  width    = 0;
    int  height   = 0;
};
```

---

## Embind bindings code

```c++
using namespace emscripten;
EMSCRIPTEN_BINDINGS( FaceDetector )
{
    value_object< EmscriptenFaceDetection >( "FaceDetection" )
        .field( "detected", &EmscriptenFaceDetection::detected )
        .field( "x"       , &EmscriptenFaceDetection::x        )
        .field( "y"       , &EmscriptenFaceDetection::y        )
        .field( "width"   , &EmscriptenFaceDetection::width    )
        .field( "height"  , &EmscriptenFaceDetection::height   );
}
```

---

## JS object usage

- `FaceDetection` is normal JS object, on JS garbage-collected heap
- developer does not need to manually call `delete`
- however, each transition of the JS ⬌ WebAssembly boundary creates a new copy

---

## Handling JS objects in C++

- [`emscripten::val` type](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#using-val-to-transliterate-javascript-to-c)
    - a proxy type to any JS object
    - provides utilities for accessing global JS properties
    - can be used to transliterate JS code into C++
- for example, obtaining width and height of the JS `ImageData` object:

```c++
void obtainImage( emscripten::val const & jsImageData )
{
    auto width = jsImageData[ "width" ].as< unsigned long >();
    auto height = jsImageData[ "height" ].as< unsigned long >();
}
```

---

## Generating modularized Javascript API

- by default, all C++ functions and objects are exported as members of global `Module` object
    - this may cause conflicts with other Emscripten-generated JS libraries
- by linking with `-s MODULARIZE=1 -s EXPORT_NAME=ModuleName` Emscripten will generate a JS function named `ModuleName` which is used for initialization of WebAssembly

---

<!-- _class: quote-large -->

# Step-by-step example

---

<!-- _class: quote-large -->

# Thank you
