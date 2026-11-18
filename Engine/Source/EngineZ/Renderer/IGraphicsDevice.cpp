#include <EngineZ/Renderer/IGraphicsDevice.h>
#include <EngineZ/Renderer/Backend/SDLGPU/SDLGPUDevice.h>

namespace EngineZ
{
	std::unique_ptr<IGraphicsDevice> CreateGraphicsDevice(Window& MainWindow)
	{
		return SDLGPUDevice::Create(MainWindow);
	}
}
