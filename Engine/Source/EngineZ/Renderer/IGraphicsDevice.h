#pragma once
#include <memory>

namespace EngineZ 
{
	struct Color
	{
		float R = 0;
		float G = 0;
		float B = 0;
		float A = 0;
	};

	class Window;

	class IGraphicsDevice
	{
	
	public:

		virtual ~IGraphicsDevice() = default;

		virtual bool BeginFrame(const Color& ClearColor) = 0;
		virtual void EndFrame() = 0;
		virtual void DrawTestTriangle() = 0;
	};

	[[nodiscard]] std::unique_ptr<IGraphicsDevice> CreateGraphicsDevice(Window& MainWindow);
}
