# Engine Z Architecture

Last updated: 19 October 2026

This document describes how the Engine Z codebase is organised and the rules it follows.

## Overview

Engine Z is a C++20 game engine for 2D and 3D games. 
It is built as a static library called `EngineZ`. 
Games are separate programs that live under `Games/` and attach to that library.

Current contents:

* `Core/Log`: `EngineZ::Log(std::string_view)` prints `[Engine Z] <message>` to standard output.
* `Sandbox`: a small test game that calls the engine.

## Repository layout

```
Engine-Z/
  README.md
  .gitignore
  CMakeLists.txt
  Engine/
    CMakeLists.txt
    Source/EngineZ/Core/Log.h
    Source/EngineZ/Core/Log.cpp
  Games/
    Sandbox/
      CMakeLists.txt
      Source/Main.cpp
  docs/
    ARCHITECTURE.md
```

* `Engine/` holds all engine code. `Source/` is the include root, and the `EngineZ/` folder inside it becomes part of every include path.
* `Games/` holds one folder per game. Each game keeps its code in its own `Source/` folder so assets can sit beside it later.
* `Docs/` holds documentation for the engine.

## The dependency rule

Games depend on the engine. The engine never depends on a game.

* Engine code never includes a file from `Games/` and never refers to a game by name.
* No `..` appears in any include path. Every include is an engine header written as `<EngineZ/...>`, a standard library header, or a third party header.

## Build system

* CMake 3.25 or newer, with C++20 (standard required, compiler extensions off).
* The root `CMakeLists.txt` sets the project and language standard, then adds `Engine` and after it `Games/Sandbox`. The order matters because games attach to the engine.
* Targets:
  * `EngineZ`: static library, built from the `.cpp` files listed in `Engine/CMakeLists.txt`. Its include folder `Source` is `PUBLIC`, so anything that links `EngineZ` can use `<EngineZ/...>` includes.
  * `Sandbox`: executable, links `EngineZ` as `PRIVATE`.

How to extend it:

* **New engine source file:** create it under `Engine/Source/EngineZ/<Module>/` and add the `.cpp` file to the `add_library(EngineZ STATIC ...)` list in `Engine/CMakeLists.txt`. Headers are not listed. CMake does not scan folders, so a `.cpp` file missing from the list is silently not built.
* **New engine module:** a new folder under `Engine/Source/EngineZ/`, handled the same way as a new file.
* **New game:** create `Games/<Name>/` with a `CMakeLists.txt` (`add_executable` and `target_link_libraries(<Name> PRIVATE EngineZ)`) and a `Source/` folder, then add `add_subdirectory(Games/<Name>)` to the root file.

## Building

Requirements: Visual Studio 2022 with the Desktop development with C++ workload (including the C++ CMake tools for Windows), Git, and CMake 3.25 or newer.

In Visual Studio:

1. File > Open > Folder, and pick the repo root.
2. Project > Configure Cache. Check View > Output with the CMake source selected.
3. Choose `Sandbox` in the toolbar's Startup Item dropdown and run with Ctrl+F5.

From the command line, in the repo root:

```
cmake -S . -B build
cmake --build build
```

Build folders:

* `build/` is used by the command line. `out/` is Visual Studio's default. Both are generated, ignored by Git, and safe to delete.
* A build folder remembers the generator it was configured with. Do not share one folder between tools, and delete it after changing generator or Visual Studio version.
* If Visual Studio does not configure CMake when the folder opens, close it, delete the hidden `.vs` folder in the repo, and reopen the folder.

## Includes and naming

* The include root is `Engine/Source`. Engine headers are included as `#include <EngineZ/Core/Log.h>`.
* All code lives in the `EngineZ` namespace.
* Headers use `.h` and sources use `.cpp`.
* Files, folders, namespaces and functions use PascalCase.
* Spelling must match exactly everywhere (CMake files, includes, folder names), because Linux treats upper and lower case as different.

## Platform policy

Windows is the primary platform. Linux and macOS are kept possible:

* Builds go through CMake only.
* No Win32 or other platform specific calls in engine code, except inside a platform layer once one exists.
* Use `std::filesystem` for paths and write paths with forward slashes.
* Any SIMD code needs a plain scalar fallback, because Apple Silicon is ARM.
* The code stays standard C++ with no compiler extensions.
* macOS is limited to OpenGL 4.1, so the renderer targets OpenGL 4.1 core as its baseline.

## Third party libraries

* A library is added only when it is needed, and its version is recorded when it is added.
* Libraries are fetched by CMake (FetchContent) at first. A package manager can be adopted later.
* Third party libraries are wrapped behind engine interfaces (for example SDL behind a platform layer, physics libraries behind physics interfaces) so they can be replaced. Third party headers stay out of public engine headers where possible.
* Math and the entity component system are written in house.

## Design principles

* Engine modules are small and independent. A module may use modules below it, never above it.
* Platform specific code is isolated in one place.
* Rendering and physics sit behind engine interfaces, not behind a library's own types.