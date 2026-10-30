#pragma once

#include <EngineZ/Renderer/Backend/SDLGPU/SDLGPUDevice.h>

namespace EngineZ
{
	class Window;

	enum GPUDevicesType
	{
		GPU_DEVICE_SDL = 0,
		GPU_DEVICE_NONE
	};

	class Renderer
	{

	public:

		Renderer(const GPUDevicesType GPUDeviceType, Window* InWindow);
		~Renderer();

		void CreateSDLGPUDevice(Window* InWindow);
	private:

		SDLGPUDevice m_SDLGPUDevice;

	};
}