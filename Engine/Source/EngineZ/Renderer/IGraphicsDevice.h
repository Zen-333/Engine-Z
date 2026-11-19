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

		/// @return True when there is something to draw into. False when the window is minimised or the frame failed.
		/// @note When this returns false, EndFrame is still safe to call and does nothing.
		[[nodiscard]] virtual bool BeginFrame(const Color& ClearColor) = 0;
		virtual void EndFrame() = 0;
		virtual void DrawTestTriangle() = 0;
	};

	[[nodiscard]] std::unique_ptr<IGraphicsDevice> CreateGraphicsDevice(Window& MainWindow);
}
