# Engine Z Architecture

Last updated: 25 October 2026

This document describes how the Engine Z codebase is organised and the rules it follows. To build the project, see the root [README](../README.md). To add code, games or libraries, see [Extending.md](Extending.md).

## Overview

Engine Z is a C++20 game engine for 2D and 3D games.
It is built as a static library called `EngineZ`.
Games are separate programs that live under `Games/` and attach to that library.
The engine uses SDL3 internally.
SDL is vendored (copied into the repo) and built as a static library together with the engine.

Current contents:

* `Core/Log`: `EngineZ::Log(std::string_view)` prints `[Engine Z] Log: <message>` and `EngineZ::LogError(std::string_view)` prints `[Engine Z] ERROR: <message>`, both to standard output.
* `Platform/Window`: owns the SDL window and event polling. The game calls `PollEvents()` once per frame and checks `ShouldClose()`. SDL stays inside `Window.cpp`.
* `ThirdParty/SDL3`: SDL 3.4.18, used by `Platform/Window`.
* `Sandbox`: a small test game that calls the engine.

## Repository layout

```
Engine-Z/
  README.md
  Engine/
    Source/EngineZ/
      Core/          logging and other foundation code
      Platform/      window and OS facing code (wraps SDL)
    ThirdParty/      vendored libraries (SDL3)
  Games/
    Sandbox/         test game
  Docs/              all documentation except the root README
```

* `Engine/` holds all engine code. `Source/` is the include root, and the `EngineZ/` folder inside it becomes part of every include path.
* `Engine/ThirdParty/` holds third party libraries, one folder per library, plus a `CMakeLists.txt` that configures and adds them.
* `Games/` holds one folder per game. Each game keeps its code in its own `Source/` folder so assets can sit beside it later.
* `Docs/` holds the documentation for the engine and the logo.

## The dependency rule

Games depend on the engine. The engine never depends on a game.

* Engine code never includes a file from `Games/` and never refers to a game by name.
* No `..` appears in any include path. Every include is an engine header written as `<EngineZ/...>`, a standard library header, or a third party header.

Inside the engine, dependencies only point down. `Platform` uses `Core`, and `Core` knows nothing about `Platform`:

```
Games/Sandbox  ->  EngineZ
                     Platform  ->  Core
                     Platform  ->  SDL3 (private)
```

## Build system

* CMake 3.25 or newer, with C++20 (standard required, compiler extensions off).
* The order matters. The root `CMakeLists.txt` adds `Engine` and then `Games/Sandbox`, because games attach to the engine. `Engine/CMakeLists.txt` adds `ThirdParty` first, because `EngineZ` links a library that `ThirdParty` creates.
* `Engine/ThirdParty/CMakeLists.txt` sets SDL's options (static library, no shared library, no tests, no install rules) and then adds `SDL3` with `add_subdirectory(SDL3 EXCLUDE_FROM_ALL)`.
* Targets:
  * `SDL3`: SDL's static library, created by SDL's own CMake files and used through the `SDL3::SDL3` target.
  * `EngineZ`: static library, built from the `.cpp` files listed in `Engine/CMakeLists.txt`. Its include folder `Source` is `PUBLIC`, so anything that links `EngineZ` can use `<EngineZ/...>` includes. It links `SDL3::SDL3` as `PRIVATE`, so only engine `.cpp` files can include SDL headers. Games cannot, and the compiler enforces this.
  * `Sandbox`: executable, links `EngineZ` as `PRIVATE`.

## Includes and naming

* The include root is `Engine/Source`. Engine headers are included as `#include <EngineZ/Core/Log.h>`.
* Third party headers are included with angle brackets, for example `#include <SDL3/SDL.h>`, and only inside engine `.cpp` files.
* All code lives in the `EngineZ` namespace.
* Headers use `.h` and sources use `.cpp`.
* Files, folders, namespaces and functions use PascalCase (except for `main.cpp`).
* Spelling must match exactly everywhere (CMake files, includes, folder names), because Linux treats upper and lower case as different.

## Platform policy

Windows is the primary platform. Linux and macOS are kept possible:

* Builds go through CMake only.
* No Win32 or other platform specific calls in engine code, except inside `Platform/`.
* Use `std::filesystem` for paths and write paths with forward slashes.
* Any SIMD code needs a plain scalar fallback, because Apple Silicon is ARM.
* The code stays standard C++ with no compiler extensions.
* macOS is limited to OpenGL 4.1, so the renderer targets OpenGL 4.1 core as its baseline.

## Third party libraries

* A library is added only when it is needed, and its version is recorded when it is added.
* Libraries are vendored: copied unmodified into `Engine/ThirdParty/<Name>/` and added with `add_subdirectory`. Building needs no internet connection, and a fresh clone builds as it is. A package manager can be adopted later.
* Third party libraries are wrapped behind engine interfaces (for example SDL behind a platform layer, physics libraries behind physics interfaces) so they can be replaced. Third party headers stay out of public engine headers where possible.
* Math and the entity component system are written in house.
* To add or upgrade a library, follow [Extending.md](Extending.md).

Current libraries:

| Library | Version | Location | Source |
|---|---|---|---|
| SDL3 | 3.4.18 (`release-3.4.18`) | `Engine/ThirdParty/SDL3` | https://github.com/libsdl-org/SDL/releases |

## Design principles

* Engine modules are small and independent. A module may use modules below it, never above it.
* Platform specific code is isolated in one place.
* Rendering and physics sit behind engine interfaces, not behind a library's own types.