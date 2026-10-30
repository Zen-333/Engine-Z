#include <EngineZ/Renderer/Backend/SDLGPU/SDLGPUDevice.h>
#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

namespace EngineZ
{
	SDLGPUDevice::SDLGPUDevice(Window* InWindow)
	{
		Init(InWindow);
	}

	void SDLGPUDevice::Init(Window* InWindow)
	{
		if (m_bHasBeenInitialized) return;

		m_Device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

		if (m_Device == nullptr)
		{
			Log("SDL GPU Device couldn't be created");
			return;
		}

		SDL_ClaimWindowForGPUDevice(m_Device, InWindow->GetSDLWindow());
		m_bHasBeenInitialized = true;

		Log("SDL GPU Device created");
	}

	SDLGPUDevice::~SDLGPUDevice()
	{
		if(m_Device != nullptr)
		{
			SDL_DestroyGPUDevice(m_Device);
			Log("SDL GPU Device Destroyed!");
		}
	}

	bool SDLGPUDevice::BeginFrame(const Color& ClearColor)
	{
		return false;
	}

	void SDLGPUDevice::EndFrame()
	{

	}

}
