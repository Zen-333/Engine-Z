#include <EngineZ/Renderer/Backend/SDLGPU/SDLGPUDevice.h>
#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <string>

namespace EngineZ
{
	static std::string ShaderFormatsToString(SDL_GPUShaderFormat Format)
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
		return false;
	}

	void SDLGPUDevice::EndFrame()
	{

	}

}
