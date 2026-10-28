#pragma once
#include <EngineZ/Renderer/GraphicsDevice.h>

namespace EngineZ
{
	class SDLGPUDevice final : public GraphicsDevice
	{

	public:

		SDLGPUDevice();
		~SDLGPUDevice() override;

		bool BeginFrame(const Color& ClearColor) override;
		void EndFrame() override;

	};
}