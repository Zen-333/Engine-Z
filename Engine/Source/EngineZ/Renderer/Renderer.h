#pragma once
#include <EngineZ/Renderer/IGraphicsDevice.h>

namespace EngineZ
{
	class Window;

	class Renderer
	{

	public:

		explicit Renderer(Window& MainWindow);
		~Renderer();

		[[nodiscard]] bool BeginFrame(const Color& ClearColor);
		void EndFrame();

		void DrawTestTriangle();

		[[nodiscard]] bool IsValid() const;

	private:

		std::unique_ptr<IGraphicsDevice> m_pDevice;

	};
}
