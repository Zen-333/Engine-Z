#pragma once
#include <EngineZ/Renderer/GraphicsDevice.h>

struct SDL_GPUDevice;

namespace EngineZ
{
	class Window;

	class SDLGPUDevice final : public GraphicsDevice
	{

	public:
		SDLGPUDevice() = default;
		SDLGPUDevice(Window* InWindow);
		~SDLGPUDevice() override;

		void Init(Window* InWindow);

		bool BeginFrame(const Color& ClearColor) override;
		void EndFrame() override;

	private:

		SDL_GPUDevice* m_Device = nullptr;
	};
}