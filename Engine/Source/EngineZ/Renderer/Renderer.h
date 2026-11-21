#pragma once

#include <EngineZ/Renderer/IGraphicsDevice.h>
#include <memory>

namespace EngineZ
{
	class Window;

	class Renderer
	{
	public:

		explicit Renderer(Window& MainWindow);

		[[nodiscard]] bool IsValid() const;
		[[nodiscard]] bool BeginFrame(const Color ClearColor);
		void EndFrame();

		void DrawTestTriangle();

	private:

		std::unique_ptr<IGraphicsDevice> m_pDevice;
	};
}
