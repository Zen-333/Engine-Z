#pragma once

#include <EngineZ/Renderer/IGraphicsDevice.h>
#include <memory>

struct SDL_Window;
struct SDL_GPUDevice;
struct SDL_GPUCommandBuffer;
struct SDL_GPURenderPass;
struct SDL_GPUBuffer;
struct SDL_GPUTransferBuffer;
struct SDL_GPUGraphicsPipeline;

namespace EngineZ
{
	class SDLGPUDevice final : public IGraphicsDevice
	{
	public:

		// ---- LIFECYCLE ----

		SDLGPUDevice(const SDLGPUDevice&) = delete;
		SDLGPUDevice(SDLGPUDevice&&) = delete;
		SDLGPUDevice& operator=(const SDLGPUDevice&) = delete;
		SDLGPUDevice& operator=(SDLGPUDevice&&) = delete;
		~SDLGPUDevice() override;

		// ---- OVERRIDES ----

		[[nodiscard]] bool BeginFrame(const Color ClearColor) override;
		void EndFrame() override;
		void DrawTestTriangle() override;

		// ---- ACTIONS ----

		/// @return The device, or nullptr if any step failed. The reason is logged and anything
		/// already created is released.
		[[nodiscard]] static std::unique_ptr<IGraphicsDevice> Create(Window& MainWindow);

	private:

		// ---- LIFECYCLE ----

		SDLGPUDevice(SDL_GPUDevice* Device, SDL_Window* MainWindow);

		// ---- HELPERS ----

		[[nodiscard]] bool CreateVertexBuffer();
		[[nodiscard]] bool CreatePipeline();

		SDL_GPUDevice* m_pDevice = nullptr;
		SDL_Window* m_pWindow = nullptr;
		SDL_GPUCommandBuffer* m_pCommandBuffer = nullptr;
		SDL_GPURenderPass* m_pRenderPass = nullptr;
		SDL_GPUBuffer* m_pVertexBuffer = nullptr;
		SDL_GPUTransferBuffer* m_pTransferBuffer = nullptr;
		SDL_GPUGraphicsPipeline* m_pGraphicsPipeline = nullptr;
	};
}
