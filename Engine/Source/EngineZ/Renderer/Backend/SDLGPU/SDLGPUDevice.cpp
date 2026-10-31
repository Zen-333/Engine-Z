#include <EngineZ/Renderer/Backend/SDLGPU/SDLGPUDevice.h>
#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

namespace EngineZ
{
	std::unique_ptr<GraphicsDevice> SDLGPUDevice::Create(Window& InWindow)
	{
		#ifdef NDEBUG
				const bool bDebugMode = false;
		#else
				const bool bDebugMode = true;
		#endif

		SDL_GPUDevice* Device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, bDebugMode, nullptr);
		if(Device == nullptr)
		{
			LogError(SDL_GetError());
			return nullptr;
		}

		SDL_Window* WindowHandle = InWindow.GetSDLWindow();
		if (!SDL_ClaimWindowForGPUDevice(Device, WindowHandle))
		{
			Log(SDL_GetError());
			SDL_DestroyGPUDevice(Device);
			return nullptr;
		}

		Log("SDL GPU Device created");

		return std::unique_ptr<GraphicsDevice>(new SDLGPUDevice(Device, WindowHandle));

	}

	SDLGPUDevice::SDLGPUDevice(SDL_GPUDevice* InDevice, SDL_Window* InWindow) : m_Device(InDevice), m_Window(InWindow)
	{

	}

	SDLGPUDevice::~SDLGPUDevice()
	{
		SDL_ReleaseWindowFromGPUDevice(m_Device, m_Window);
		SDL_DestroyGPUDevice(m_Device);
		Log("SDL GPU Device destroyed");
	}

	bool SDLGPUDevice::BeginFrame(const Color& ClearColor)
	{
		return false;
	}

	void SDLGPUDevice::EndFrame()
	{

	}

}
