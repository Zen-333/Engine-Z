# Engine Z Architecture

Last updated: 22 October 2026

This document describes how the Engine Z codebase is organised and the rules it follows.

## Overview

Engine Z is a C++20 game engine for 2D and 3D games. 
It is built as a static library called `EngineZ`. 
Games are separate programs that live under `Games/` and attach to that library.
The engine uses SDL3 internally. 
SDL is vendored (copied into the repo) and built as a static library together with the engine.

Current contents:

* `Core/Log`: `EngineZ::Log(std::string_view)` prints `[Engine Z] Log: <message>` and `EngineZ::LogError(std::string_view)` prints `[Engine Z] ERROR: <message>`, both to standard output.
* `ThirdParty/SDL3`: SDL 3.4.18, linked into the engine but not yet used by any engine code.
* `Sandbox`: a small test game that calls the engine.

## Repository layout

```
Engine-Z/
  README.md
  .gitignore
  .gitattributes
  CMakeLists.txt
  Engine-Z-Logo.jpg
  Engine/
    CMakeLists.txt
    Source/EngineZ/Core/Log.h
    Source/EngineZ/Core/Log.cpp
    ThirdParty/
      CMakeLists.txt
      SDL3/                  (SDL 3.4.18, copied in unmodified)
  Games/
    Sandbox/
      CMakeLists.txt
      Source/main.cpp
  Docs/
    Architecture.md
```

* `Engine/` holds all engine code. `Source/` is the include root, and the `EngineZ/` folder inside it becomes part of every include path.
* `Engine/ThirdParty/` holds third party libraries, one folder per library, plus a `CMakeLists.txt` that configures and adds them.
* `Games/` holds one folder per game. Each game keeps its code in its own `Source/` folder so assets can sit beside it later.
* `Docs/` holds documentation for the engine.

## The dependency rule

Games depend on the engine. The engine never depends on a game.

* Engine code never includes a file from `Games/` and never refers to a game by name.
* No `..` appears in any include path. Every include is an engine header written as `<EngineZ/...>`, a standard library header, or a third party header.

## Build system

* CMake 3.25 or newer, with C++20 (standard required, compiler extensions off).
* The root `CMakeLists.txt` sets the project and language standard, then adds `Engine` and after it `Games/Sandbox`. The order matters because games attach to the engine.
* `Engine/CMakeLists.txt` adds `ThirdParty` first, because `EngineZ` links a library that `ThirdParty` creates.
* `Engine/ThirdParty/CMakeLists.txt` sets SDL's options (static library, no shared library, no tests, no install rules) and then adds `SDL3` with `add_subdirectory(SDL3 EXCLUDE_FROM_ALL)`.
* Targets:
  * `SDL3`: SDL's static library, created by SDL's own CMake files and used through the `SDL3::SDL3` target.
  * `EngineZ`: static library, built from the `.cpp` files listed in `Engine/CMakeLists.txt`. Its include folder `Source` is `PUBLIC`, so anything that links `EngineZ` can use `<EngineZ/...>` includes. It links `SDL3::SDL3` as `PRIVATE`, so only engine `.cpp` files can include SDL headers. Games cannot, and the compiler enforces this.
  * `Sandbox`: executable, links `EngineZ` as `PRIVATE`

How to extend it:

* **New engine source file:** create it under `Engine/Source/EngineZ/<Module>/` and add the `.cpp` file to the `add_library(EngineZ STATIC ...)` list in `Engine/CMakeLists.txt`. Headers are not listed. CMake does not scan folders, so a `.cpp` file missing from the list is silently not built.
* **New engine module:** a new folder under `Engine/Source/EngineZ/`, handled the same way as a new file.
* **New game:** create `Games/<Name>/` with a `CMakeLists.txt` (`add_executable` and `target_link_libraries(<Name> PRIVATE EngineZ)`) and a `Source/` folder, then add `add_subdirectory(Games/<Name>)` to the root file.
* **New third party library:** follow the steps in the Third party libraries section, set its options and add it in `Engine/ThirdParty/CMakeLists.txt`, then link it in `Engine/CMakeLists.txt` as `PRIVATE`. Use `PUBLIC` only if a public engine header needs the library's types.

## Building

Requirements: Visual Studio 2022 with the Desktop development with C++ workload (including the C++ CMake tools for Windows), Git, and CMake 3.25 or newer.

No download step is needed, because SDL is vendored. The first build compiles all of SDL, so it is slower than later builds.

In Visual Studio:

1. File > Open > Folder, and pick the repo root.
2. Project > Configure Cache. Check View > Output with the CMake source selected.
3. Choose `Sandbox` in the toolbar's Startup Item dropdown and run with Ctrl+F5.

From the command line, in the repo root:

```
cmake -S . -B build
cmake --build build
```

On Linux, SDL needs some system packages (X11 or Wayland development libraries) before it configures. 
o#The list is in `Engine/ThirdParty/SDL3/docs/README-linux.md`.

Build folders:

* `build/` is used by the command line. `out/` is Visual Studio's default. Both are generated, ignored by Git, and safe to delete.
* A build folder remembers the generator it was configured with. Do not share one folder between tools, and delete it after changing generator or Visual Studio version.
* If Visual Studio does not configure CMake when the folder opens, close it, delete the hidden `.vs` folder in the repo, and reopen the folder.

## Includes and naming

* The include root is `Engine/Source`. Engine headers are included as `#include <EngineZ/Core/Log.h>`.
* Third party headers are included with angle brackets, for example `#include <SDL3/SDL.h>`, and only inside engine `.cpp` files.
* All code lives in the `EngineZ` namespace.
* Headers use `.h` and sources use `.cpp`.
* Files, folders, namespaces and functions use PascalCase (except for main.cpp).
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
* Libraries are vendored: copied unmodified into `Engine/ThirdParty/<Name>/` and added with `add_subdirectory`. Building needs no internet connection, and a fresh clone builds as it is. A package manager can be adopted later.
* Third party libraries are wrapped behind engine interfaces (for example SDL behind a platform layer, physics libraries behind physics interfaces) so they can be replaced. Third party headers stay out of public engine headers where possible.
* Math and the entity component system are written in house.

Current libraries:
 
| Library | Version | Location | Source |
|---|---|---|---|
| SDL3 | 3.4.18 (`release-3.4.18`) | `Engine/ThirdParty/SDL3` | https://github.com/libsdl-org/SDL/releases |

Adding or upgrading a vendored library:
 
1. Download the release source for the exact version, and copy it into `Engine/ThirdParty/<Name>/` so that its own `CMakeLists.txt` sits directly inside that folder. Replace the old folder when upgrading, and do not edit the files.
2. Stage it with `git add -f Engine/ThirdParty/<Name>`. Libraries ship their own `.gitignore` files, and Git obeys those silently. For example, SDL's `src/render/gpu/shaders/.gitignore` ignores `.hlsl` files, so a plain `git add` would leave them out. The `-f` flag overrides that.
3. Compare the number of tracked files with the number of files on disk. They must match.
```
   git ls-files Engine/ThirdParty/<Name> | find /c /v ""
   dir /s /b /a-d Engine\ThirdParty\<Name> | find /c /v ""
```
4. Update the table above with the new version.

## Design principles

* Engine modules are small and independent. A module may use modules below it, never above it.
* Platform specific code is isolated in one place.
* Rendering and physics sit behind engine interfaces, not behind a library's own types.