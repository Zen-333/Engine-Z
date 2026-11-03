# Engine Z Renderer

Last updated: 3 November 2026

How rendering is structured, and the rules that keep the graphics API replaceable. For the wider codebase, see [Architecture.md](Architecture.md).

Current state: the renderer clears the window every frame. There are no shaders, buffers or drawing yet.

## Layers

```
Game code
  ->  Renderer                  Renderer/Renderer.h
        ->  GraphicsDevice      Renderer/GraphicsDevice.h  (interface, engine types only)
              ^
              |  implements
        SDLGPUDevice            Renderer/Backend/SDLGPU/   (the only place SDL_GPU is used)
```

| File | Job |
|---|---|
| `Renderer.h / .cpp` | What the game calls: `BeginFrame` and `EndFrame`. Owns one `GraphicsDevice`. |
| `GraphicsDevice.h` | The interface a graphics backend must implement, plus `CreateGraphicsDevice`. |
| `GraphicsDevice.cpp` | The one place that decides which backend runs. |
| `Backend/SDLGPU/SDLGPUDevice.h / .cpp` | The SDL_GPU backend. |

## Rules

* No SDL type and no SDL include appears in any header under `Renderer/`, except inside `Backend/`. Public headers use engine types such as `Color`.
* Only `Backend/SDLGPU/` includes SDL GPU headers. The one exception is `Platform/Window`, which exposes `GetSDLWindow()` so the backend can claim the window.
* Only `CreateGraphicsDevice` knows which backend exists. Nothing else names a backend class.
* `CreateGraphicsDevice` returns `nullptr` if the device cannot be created, so there is no half built device.
* The interface grows only when something needs it.

These rules are checked on every commit by `.githooks/pre-commit`.

## Creating the device

`SDLGPUDevice::Create` does the following:

1. Calls `SDL_CreateGPUDevice` with SPIR-V as the only shader format, debug mode on unless `NDEBUG` is defined, and no driver name, so SDL chooses.
2. Claims the window with `SDL_ClaimWindowForGPUDevice`. SDL starts the swapchain with SDR colour and VSync on.
3. Logs the driver and the shader formats the device accepts.

Because only SPIR-V is offered, SDL can only choose its Vulkan backend. This was confirmed on the development machine: the driver is `vulkan` and the formats are SPIR-V. Forcing `direct3d12` fails with "unsupported", because Direct3D 12 needs DXBC or DXIL shaders. macOS is not supported yet, because Metal needs MSL shaders.

## A frame

`BeginFrame` returns `true` when there is something to draw into:

1. Acquire a command buffer.
2. Wait for and acquire the swapchain texture. This blocks until one is free, which paces the loop to the monitor with VSync.
3. Begin a render pass that clears the texture to the given colour.

`EndFrame` ends the render pass and submits the command buffer. Submitting also presents the swapchain texture, so there is no separate swap call.

`BeginFrame` returns `false` when nothing can be drawn, for example when the window is minimised and SDL gives no texture. The game skips drawing and sleeps briefly so the loop does not spin. `EndFrame` is safe to call anyway and does nothing.

```cpp
while (!MainWindow.ShouldClose())
{
	MainWindow.PollEvents();

	if (!MainRenderer.BeginFrame({ 0.4f, 0.4f, 1.0f, 1.0f }))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	// Drawing goes here, between BeginFrame and EndFrame

	MainRenderer.EndFrame();
}
```

## Known limitations

* The picture freezes while a window border is held. Windows runs its own loop during a resize. The fix is `SDL_AddEventWatch` with `SDL_EVENT_WINDOW_EXPOSED`, once the engine owns the main loop.
* No shaders or drawing yet. The shader workflow (SDL_shadercross, and whether to ship DXIL as well as SPIR-V) is decided in the next milestone.
* Viewport and projection handling arrive with the first drawing.
* Direct3D 12 and Metal are not enabled, see above.

## Adding a backend

For example a hand written Vulkan backend:

1. Create `Renderer/Backend/<Name>/` with a class that derives from `GraphicsDevice` and has a static `Create(Window&)`.
2. Add its `.cpp` files to the list in `Engine/CMakeLists.txt`.
3. Change `CreateGraphicsDevice` to pick it. This is the only place that changes.
4. Keep the new API's types inside that folder, and update the rules in `.githooks/pre-commit` if the backend brings in a new library.
5. Supply shaders in the format the new backend consumes. Vulkan takes SPIR-V.