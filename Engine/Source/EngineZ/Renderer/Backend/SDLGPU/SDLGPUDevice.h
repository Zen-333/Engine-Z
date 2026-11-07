#pragma once
#include <EngineZ/Renderer/GraphicsDevice.h>

struct SDL_GPUDevice;
struct SDL_Window;
struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;
struct SDL_GPUBuffer;
struct SDL_GPUTransferBuffer;

namespace EngineZ
{

	class SDLGPUDevice final : public GraphicsDevice
	{

	public:
		static std::unique_ptr<GraphicsDevice> Create(Window& InWindow);

		~SDLGPUDevice() override;

		bool BeginFrame(const Color& ClearColor) override;
		void EndFrame() override;

	private:

		SDLGPUDevice(SDL_GPUDevice* InDevice, SDL_Window* InWindow);
		bool CreateVertexBuffer();

		SDL_GPUDevice* m_Device = nullptr;
		SDL_Window* m_Window = nullptr;
		SDL_GPUCommandBuffer* m_CommandBuffer = nullptr;
		SDL_GPURenderPass* m_RenderPass = nullptr;
		SDL_GPUBuffer* m_VertexBuffer = nullptr;
		SDL_GPUTransferBuffer* m_TransferBuffer = nullptr;
	};
}