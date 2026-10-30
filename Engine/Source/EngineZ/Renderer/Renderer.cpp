#include <EngineZ/Renderer/Renderer.h>
#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>

namespace EngineZ
{
	Renderer::Renderer(const GPUDevicesType GPUDeviceType, Window* InWindow)
	{
		if(InWindow == nullptr)
		{
			LogError("Window provided to renderer is null.");
			return;
		}

		switch (GPUDeviceType)
		{
			case GPU_DEVICE_SDL:
				CreateSDLGPUDevice(InWindow);
				break;
			case GPU_DEVICE_NONE:
				LogError("No GPU Device type provided to renderer.");
				return;
		}
		
	}

	Renderer::~Renderer()
	{

	}

	void Renderer::CreateSDLGPUDevice(Window* InWindow)
	{
		m_SDLGPUDevice.Init(InWindow);
	}
}