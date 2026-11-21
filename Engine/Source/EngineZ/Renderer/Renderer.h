#pragma once

#include <EngineZ/Renderer/IGraphicsDevice.h>
#include <memory>

namespace EngineZ
{
	class Window;

	class Renderer
	{
	public:

		/// @note If the graphics device cannot be created, the reason is logged and IsValid returns false.
		explicit Renderer(Window& MainWindow);

		/// @return False when the graphics device could not be created. BeginFrame then returns false
		/// and every other Renderer function does nothing.
		[[nodiscard]] bool IsValid() const;
		[[nodiscard]] bool BeginFrame(const Color ClearColor);
		void EndFrame();

		void DrawTestTriangle();

	private:

		std::unique_ptr<IGraphicsDevice> m_pDevice;
	};
}
