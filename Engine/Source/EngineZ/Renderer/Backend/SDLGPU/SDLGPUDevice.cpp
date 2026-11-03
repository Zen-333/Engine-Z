#include <EngineZ/Renderer/Backend/SDLGPU/SDLGPUDevice.h>
#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <string>

namespace EngineZ
{
	static std::string ShaderFormatsToString(const SDL_GPUShaderFormat Format)
	{
		struct FormatName 
		{
			SDL_GPUShaderFormat Flag;
			const char* Name;
		};

		static constexpr FormatName Names[] =
		{
			{ SDL_GPU_SHADERFORMAT_SPIRV,    "SPIR-V"   },
			{ SDL_GPU_SHADERFORMAT_DXBC,     "DXBC"     },
			{ SDL_GPU_SHADERFORMAT_DXIL,     "DXIL"     },
			{ SDL_GPU_SHADERFORMAT_MSL,      "MSL"      },
			{ SDL_GPU_SHADERFORMAT_METALLIB, "METALLIB" },
		};

		std::string Result;

		for (const FormatName& Entry : Names)
		{	
			if(Entry.Flag & Format)
			{
				if(!Result.empty())
				{
					Result += " | ";
				}
				Result += Entry.Name;
			}

		}

		if(Result.empty())
		{
			Result = "The device reported no formats I know about";
		}

		return Result;
		
	}

	std::unique_ptr<GraphicsDevice> SDLGPUDevice::Create(Window& InWindow)
	{
		#ifdef NDEBUG
				const bool bDebugMode = false;
		#else
				const bool bDebugMode = true;
		#endif

		SDL_GPUDevice* Device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, bDebugMode, nullptr);
		if(Device == nullptr)
		{
			LogError(SDL_GetError());
			return nullptr;
		}

		SDL_Window* WindowHandle = InWindow.GetSDLWindow();
		if (!SDL_ClaimWindowForGPUDevice(Device, WindowHandle))
		{
			LogError(SDL_GetError());
			SDL_DestroyGPUDevice(Device);
			return nullptr;
		}

		Log("SDL GPU Device created");
		Log(std::string("GPU driver: ") + SDL_GetGPUDeviceDriver(Device));
		Log("Shader formats: " + ShaderFormatsToString(SDL_GetGPUShaderFormats(Device)));
		return std::unique_ptr<GraphicsDevice>(new SDLGPUDevice(Device, WindowHandle));

	}

	SDLGPUDevice::SDLGPUDevice(SDL_GPUDevice* InDevice, SDL_Window* InWindow) : m_Device(InDevice), m_Window(InWindow)
	{

	}

	SDLGPUDevice::~SDLGPUDevice()
	{
		SDL_ReleaseWindowFromGPUDevice(m_Device, m_Window);
		SDL_DestroyGPUDevice(m_Device);
		Log("SDL GPU Device destroyed");
	}

	bool SDLGPUDevice::BeginFrame(const Color& ClearColor)
	{
		// Known: the picture freezes while a window border is held (Windows runs its own loop). 
		// Fix later with SDL_AddEventWatch and SDL_EVENT_WINDOW_EXPOSED when the engine owns the main loop.

		// Viewport and projection handling arrive in M4

		m_CommandBuffer = SDL_AcquireGPUCommandBuffer(m_Device);

		if(m_CommandBuffer == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		SDL_GPUTexture* SwapChainTexture = nullptr;
		Uint32 Width, Height;

		if(!SDL_WaitAndAcquireGPUSwapchainTexture(m_CommandBuffer, m_Window, &SwapChainTexture, &Width, &Height))
		{
			LogError(SDL_GetError());
			SDL_SubmitGPUCommandBuffer(m_CommandBuffer);
			m_CommandBuffer = nullptr;
			return false;
		}

		if(SwapChainTexture == nullptr)
		{
			SDL_SubmitGPUCommandBuffer(m_CommandBuffer);
			m_CommandBuffer = nullptr;
			return false;
		}

		SDL_GPUColorTargetInfo ColorTargetInfo{};
		ColorTargetInfo.clear_color = { ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A };
		ColorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		ColorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
		ColorTargetInfo.texture = SwapChainTexture;

		m_RenderPass = SDL_BeginGPURenderPass(m_CommandBuffer, &ColorTargetInfo, 1, nullptr);

		if(m_RenderPass == nullptr)
		{
			LogError(SDL_GetError());
			SDL_SubmitGPUCommandBuffer(m_CommandBuffer);
			m_CommandBuffer = nullptr;
			return false;
		}
		return true;
	}

	void SDLGPUDevice::EndFrame()
	{
		if (m_RenderPass == nullptr) return;

		SDL_EndGPURenderPass(m_RenderPass);
		SDL_SubmitGPUCommandBuffer(m_CommandBuffer);

		m_RenderPass = nullptr;
		m_CommandBuffer = nullptr;
	}

}
