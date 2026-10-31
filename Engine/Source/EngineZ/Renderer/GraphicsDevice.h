#pragma once
#include <memory>

namespace EngineZ 
{
	struct Color { float R; float G; float B; float A; };

	class Window;

	class GraphicsDevice
	{
	
	public:

		virtual ~GraphicsDevice() = default;

		virtual bool BeginFrame(const Color& ClearColor) = 0;
		virtual void EndFrame() = 0;


	};

	std::unique_ptr<GraphicsDevice> CreateGraphicsDevice(Window& InWindow);
}
