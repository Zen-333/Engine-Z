# Engine Z Renderer

Last updated: 21 November 2026

How rendering is structured, and the rules that keep the graphics API replaceable. 
For the wider codebase, see [Architecture.md](Architecture.md).

Current state: 
the renderer clears the window every frame and draws one coloured triangle with a shader pipeline. 
The triangle comes from a temporary function, `DrawTestTriangle`, that goes away when real mesh types arrive.

## Layers

```
Game code
  ->  Renderer                  Renderer/Renderer.h
        ->  IGraphicsDevice     Renderer/IGraphicsDevice.h  (interface, engine types only)
              ^
              |  implements
        SDLGPUDevice            Renderer/Backend/SDLGPU/   (the only place SDL_GPU is used)
```

| File | Job |
|---|---|
| `Renderer.h / .cpp` | What the game calls. Owns one `IGraphicsDevice`. |
| `IGraphicsDevice.h` | The interface a graphics backend must implement, plus the `Color` type. |
| `IGraphicsDevice.cpp` | Holds `CreateGraphicsDevice`, the one place that decides which backend runs. |
| `Backend/SDLGPU/SDLGPUDevice.h / .cpp` | The SDL_GPU backend. |

## Rules

* No SDL type and no SDL include appears in any header under `Renderer/`, except inside `Backend/`. Public headers use engine types such as `Color`.
* Only `Backend/SDLGPU/` includes SDL GPU headers. The one exception is `Platform/Window`, which exposes `GetSDLWindow()` so the backend can claim the window.
* Only `CreateGraphicsDevice` knows which backend exists. Nothing else names a backend class.
* `CreateGraphicsDevice` returns `nullptr` if the device cannot be created, so there is no half built device. `Renderer::IsValid()` reports this, and the Sandbox exits with code 1 when it is false.
* The interface grows only when something needs it.

These rules are checked on every commit by `.githooks/pre-commit`.

## Creating the device

`SDLGPUDevice::Create` does the following, and returns `nullptr` as soon as any step fails:

1. Calls `SDL_CreateGPUDevice` with SPIR-V as the only shader format, debug mode on unless `NDEBUG` is defined, and no driver name, so SDL chooses.
2. Claims the window with `SDL_ClaimWindowForGPUDevice`. SDL starts the swapchain with SDR colour and VSync on.
3. Logs the driver and the shader formats the device accepts.
4. Creates the vertex buffer and uploads the triangle's vertices to it.
5. Loads the two shaders and builds the graphics pipeline.

Because only SPIR-V is offered, SDL can only choose its Vulkan backend. 
This was confirmed on the development machine: the driver is `vulkan` and the formats are SPIR-V.
macOS is not supported yet, because Metal needs MSL shaders.

## Shaders

* The shader source is GLSL 450 in `Engine/Shaders/Source/` (`Triangle.vert` and `Triangle.frag`).
* It is compiled to SPIR-V with `glslc` from the Vulkan SDK, run from that source folder:

```
glslc Triangle.vert -o ../Compiled/SPIRV/Triangle.vert.spv
glslc Triangle.frag -o ../Compiled/SPIRV/Triangle.frag.spv
```

* The `.spv` files in `Engine/Shaders/Compiled/SPIRV/` are committed, so building the engine does not need the Vulkan SDK.
* The `CopyShaders` target in `Games/Sandbox/CMakeLists.txt` copies that folder next to the executable on every build.
* At run time the backend reads `Shaders/SPIRV/<name>.spv` relative to the executable, so the game works from any working directory.

## The triangle

* The vertex buffer holds three vertices. Each has a position (three floats) and a colour (four floats).
* The graphics pipeline tells the GPU how to read that buffer, which two shaders to run, that the primitives are triangle lists, and that it draws into one colour target in the swapchain's format. Everything else stays at SDL's zero default: filled triangles, no culling, one sample, no depth or stencil.
* The vertices are uploaded once, through a transfer buffer and a copy pass. The transfer buffer is released afterwards.

## A frame

`BeginFrame` returns `true` when there is something to draw into:

1. Acquire a command buffer.
2. Wait for and acquire the swapchain texture. This blocks until one is free, which paces the loop to the monitor with VSync.
3. Begin a render pass that clears the texture to the given colour.

`DrawTestTriangle` then binds the pipeline and the vertex buffer and draws three vertices. 
`EndFrame` ends the render pass and submits the command buffer. 
Submitting also presents the swapchain texture, so there is no separate swap call.

`BeginFrame` returns `false` when nothing can be drawn, for example when the window is minimised and SDL gives no texture. 
The game skips drawing and sleeps briefly so the loop does not spin. 
`DrawTestTriangle` and `EndFrame` are safe to call anyway and do nothing. 
See `Games/Sandbox/Source/Main.cpp` for the loop.

## Known limitations

* The picture freezes while a window border is held. Windows runs its own loop during a resize. The fix is `SDL_AddEventWatch` with `SDL_EVENT_WINDOW_EXPOSED`, once the engine owns the main loop.
* The triangle stretches when the window is resized, because there is no projection yet. Viewport and projection handling come next.
* `DrawTestTriangle` is temporary and leaves the interface when real mesh types arrive.

## Adding a backend

For example a hand written Vulkan backend:

1. Create `Renderer/Backend/<Name>/` with a class that derives from `IGraphicsDevice` and has a static `Create(Window&)`.
2. Add its `.cpp` files to the list in `Engine/CMakeLists.txt`.
3. Change `CreateGraphicsDevice` to pick it. This is the only place that changes.
4. Keep the new API's types inside that folder, and update the rules in `.githooks/pre-commit` if the backend brings in a new library.
5. Supply shaders in the format the new backend consumes. Vulkan takes SPIR-V.
