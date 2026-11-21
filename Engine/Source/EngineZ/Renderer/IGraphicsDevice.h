#pragma once

#include <memory>

namespace EngineZ
{
	struct Color
	{
		float R = 0.0f;
		float G = 0.0f;
		float B = 0.0f;
		float A = 0.0f;
	};

	class Window;

	class IGraphicsDevice
	{
	public:

		virtual ~IGraphicsDevice() = default;

		/// @return True when there is something to draw into. False when the window is minimised or the frame failed.
		/// @note When this returns false, EndFrame is still safe to call and does nothing.
		[[nodiscard]] virtual bool BeginFrame(const Color ClearColor) = 0;
		virtual void EndFrame() = 0;
		virtual void DrawTestTriangle() = 0;
	};

	/// @return The device for the chosen backend, or nullptr if it could not be created. The reason is logged.
	[[nodiscard]] std::unique_ptr<IGraphicsDevice> CreateGraphicsDevice(Window& MainWindow);
}
