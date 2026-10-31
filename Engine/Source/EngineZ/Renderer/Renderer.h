#pragma once
#include <EngineZ/Renderer/GraphicsDevice.h>

namespace EngineZ
{
	class Window;

	class Renderer
	{

	public:

		explicit Renderer(Window& InWindow);
		~Renderer();

	private:

		std::unique_ptr<GraphicsDevice> m_Device;

	};
}