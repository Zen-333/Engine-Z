#pragma once
#include <EngineZ/Renderer/GraphicsDevice.h>
#include <SDL3/SDL.h>

namespace EngineZ
{
	class Window;

	class Renderer
	{

	public:

		explicit Renderer(Window& InWindow);
		~Renderer();

		bool BeginFrame(const Color& ClearColor);
		void EndFrame();

	private:

		std::unique_ptr<GraphicsDevice> m_Device;

	};
}