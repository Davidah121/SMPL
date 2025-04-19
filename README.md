# SMPL

A simple but full featured C++ library designed to simplify making a variety of applications across Windows and Linux while being simple to understand the source code.

Due to it simplicity, its performance is not cutting edge nor does it contain everything a speciallized library would have but it is very usable for many applications that aren't attempting to push the boundaries of what is possible.

## Features

While the library is intended to be more general purpose at this point, newer features will pivot towards a Computer Graphics and Computer Vision focus. With that in mind a quick overview:

- An extended set of math operations featuring more advance math classes, iterative methods to solve equations that may not have an analytical solution available, and commonly desired function for those extra classes or for analysis.
  - Vector classes, Matrix classes, Fourier Transform, Clustering
  - SIMD and OpenMP support for matrix operations.
- Xinput, DirectInput, Keyboard and Mouse input mechanisms.
- XML, HTML, JSON, support.
- Socket programming, Websockets, and a HTTP server and client
- Image loading, saving, and processing for common formats and common features.
  - Convolutions, Cross Correlation, Canny edge detection, Boundary tracing, standard methods for drawing shapes, Image Hashes, etc.
- Compression methods
  - RLE, LZW, LZSS, LZ77, DEFLATE, Huffman encoding, Length limited Huffman encoding
  - Note that these can be used in an online fashion and have been optimized somewhat and are not equivalent to a naive approach.
- Audio
- Neural Networks
  - Newer framework allowing for easier extension.
- A cross platform GUI
  - The gui does not rely on a specific renderer. Just needs a set of specific functions implemented in GraphicsInterface and software rendering will always be available.
- OpenMP support and hybrid Mutexes and hybrid Semaphores are available
  - Job queues are available even an adaptable one.
- A simpler to understand, use, and robust Smart Memory system
  - Existing system using unique pointers or shared pointers fails to work with memory created on the stack.
  - The new system allows memory to be deleted safely, provided you correctly check if memory is valid before using it, regardless of what holds onto it fixing a different type of "memory leak".
    - memory that should've been freed but is contained in some list doing nothing preventing it from being deleted.
  - Serialization of pointers uses this Smart Memory system to ensure local pointers aren't serialized.

## Requirements

The requirements change based on the preprocessor values set. To build the full library, the Windows SDK or its substitute with MinGW is needed. If building for Linux, pthread, x11 are needed.

OpenSSL is needed if SSL is needed (This includes HTTPS).

GLEW is needed if OpenGL is used.

OpenMP and SSE or AVX2 are not required but highly recommended.

[SEML](https://github.com/Davidah121/SEML) is required.

[SimpleSerialization](https://github.com/Davidah121/SimpleSerialization) is required.

If the unit test are desired, [Catch2](https://github.com/catchorg/Catch2) is required.

## Compilation Details

The following preprocessor variables are utilized to determine the library's features:

- \_\_unix\_\_
  - Typically set by the compiler based on the system its being compiled on. Once set, the library will disable Windows only features and provide a Linux version for as much as possible
- _WIN32
  - Typically set by the compiler based on the system its being compiled on. Once set, the library will use a Windows version for all features and allow all optional features. This is not mean that the Windows SDK is required for compilation but it will be required if a desired feature needs it.
- USE_OPENSSL
  - Allows OpenSSL to be used to provide secure sockets allowing HTTPS to work.
- USE_OPENGL
  - Allows OpenGL to be used. Assume the latest version of OpenGL is needed. GLEW is required.
- USE_PREPROCESSOR_TRICKS
  - When set to 1, the preprocessor tricks used to make SimpleSerialization work will be used. This is detailed more in that library. Should be set to 1 but at compile time and not in the source code.
- USE_DIRECTX
  - Allows DirectX to be used. Specifically DirectX11. Does not work on Linux. The Windows SDK is required.
- DLL_EXPORT / DLL_IMPORT
  - Allows the library to be compiled out to a dll instead of as a static library.
- OPTI
  - Can be set to 0, 1, or 2 which will determine whether to use SSE4.2, AVX2, or no SIMD functions
    - Subject to change. May be removed all together as the compiler still needs sse or avx flags set
- NO_GAME_INPUT
  - Disables XInput and DInput classes if they are not needed or can't be used on the desired platform. Neither class works on Linux
- NO_SOCKETS
  - Disables Network Sockets if not needed. Does work in both Windows and Linux.
- NO_WINDOW
  - Disables SimpleWindow class if not needed. Works on both Linux and Windows. Requires X11 or the Windows SDK.
  - Note that OpenGL and DirectX require the SimpleWindow class
- NO_AUDIO
  - Disables Audio playback and recording if not needed. Only works on Windows currently.
  - Note that this does not remove the loading of Audio media so the Sound class still works and can be used for data processing.
