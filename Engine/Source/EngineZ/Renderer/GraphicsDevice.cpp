#include <EngineZ/Renderer/GraphicsDevice.h>
#include <EngineZ/Renderer/Backend/SDLGPU/SDLGPUDevice.h>

namespace EngineZ
{
	std::unique_ptr<GraphicsDevice> CreateGraphicsDevice(Window& InWindow)
	{
		return SDLGPUDevice::Create(InWindow);
	}
}
