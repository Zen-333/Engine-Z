#pragma once
#include <EngineZ/Renderer/GraphicsDevice.h>

struct SDL_Window;
struct SDL_GPUDevice;
struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;
struct SDL_GPUBuffer;
struct SDL_GPUTransferBuffer;
struct SDL_GPUGraphicsPipeline;

namespace EngineZ
{

	class SDLGPUDevice final : public GraphicsDevice
	{

	public:
		static std::unique_ptr<GraphicsDevice> Create(Window& MainWindow);

		~SDLGPUDevice() override;

		bool BeginFrame(const Color& ClearColor) override;
		void EndFrame() override;

		virtual void DrawTestTriangle() override;

	private:

		SDLGPUDevice(SDL_GPUDevice* Device, SDL_Window* MainWindow);
		bool CreateVertexBuffer();
		bool CreatePipeline();

		SDL_GPUDevice* m_pDevice = nullptr;
		SDL_Window* m_pWindow = nullptr;
		SDL_GPUCommandBuffer* m_pCommandBuffer = nullptr;
		SDL_GPURenderPass* m_pRenderPass = nullptr;
		SDL_GPUBuffer* m_pVertexBuffer = nullptr;
		SDL_GPUTransferBuffer* m_pTransferBuffer = nullptr;
		SDL_GPUGraphicsPipeline* m_pGraphicsPipeline = nullptr;
	};
}