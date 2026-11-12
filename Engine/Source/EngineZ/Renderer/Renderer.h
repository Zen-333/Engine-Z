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

		bool BeginFrame(const Color& ClearColor);
		void EndFrame();

		void DrawTestTriangle();

		bool IsValid() const;

	private:

		std::unique_ptr<GraphicsDevice> m_Device;

	};
}