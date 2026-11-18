#include <EngineZ/Renderer/Backend/SDLGPU/SDLGPUDevice.h>
#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <string>
#include <cstddef>

namespace EngineZ
{
	struct Vertex
	{
		float X, Y, Z;
		float R, G, B, A;
	};

	static Vertex Vertices[]
	{
		{0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},    // Top vertex
		{-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},  // Bottom left vertex
		{0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}    // Bottom right vertex
	};

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

	static SDL_GPUShader* LoadShader(SDL_GPUDevice* Device, const std::string& FileName, SDL_GPUShaderStage Stage)
	{
		const char* BasePath = SDL_GetBasePath();
		if(BasePath == nullptr)
		{
			LogError(SDL_GetError());
			return nullptr;
		}

		const std::string Path = std::string(BasePath) + "Shaders/SPIRV/" + FileName;

		size_t CodeSize = 0;
		void* Code = SDL_LoadFile(Path.c_str(), &CodeSize);
		if(Code == nullptr)
		{
			LogError(SDL_GetError());
			return nullptr;
		}

		SDL_GPUShaderCreateInfo Info{};
		Info.code = static_cast<const Uint8*>(Code);
		Info.code_size = CodeSize;
		Info.entrypoint = "main";
		Info.format = SDL_GPU_SHADERFORMAT_SPIRV;
		Info.stage = Stage;
		SDL_GPUShader* Shader = SDL_CreateGPUShader(Device, &Info);

		SDL_free(Code);

		if(Shader == nullptr)
		{
			LogError(SDL_GetError());
			return nullptr;
		}

		return Shader;
	}

	std::unique_ptr<GraphicsDevice> SDLGPUDevice::Create(Window& MainWindow)
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

		SDL_Window* WindowHandle = MainWindow.GetSDLWindow();
		if (!SDL_ClaimWindowForGPUDevice(Device, WindowHandle))
		{
			LogError(SDL_GetError());
			SDL_DestroyGPUDevice(Device);
			return nullptr;
		}

		Log("SDL GPU Device created");
		Log(std::string("GPU driver: ") + SDL_GetGPUDeviceDriver(Device));
		Log("Shader formats: " + ShaderFormatsToString(SDL_GetGPUShaderFormats(Device)));

		std::unique_ptr<SDLGPUDevice> NewDevice(new SDLGPUDevice(Device, WindowHandle));

		if(!NewDevice->CreateVertexBuffer())
		{
			LogError("Vertex buffer couldn't be created");
			return nullptr;
		}
		if(!NewDevice->CreatePipeline())
		{
			LogError("Shaders couldn't be created");
			return nullptr;
		}

		Log("Vertex buffer and shaders created");
		return NewDevice;
	}

	SDLGPUDevice::SDLGPUDevice(SDL_GPUDevice* Device, SDL_Window* MainWindow) : m_pDevice(Device), m_pWindow(MainWindow)
	{
	}

	SDLGPUDevice::~SDLGPUDevice()
	{
		if(m_pGraphicsPipeline)
		{
			SDL_ReleaseGPUGraphicsPipeline(m_pDevice, m_pGraphicsPipeline);
		}
		if(m_pVertexBuffer)
		{
			SDL_ReleaseGPUBuffer(m_pDevice, m_pVertexBuffer);
		}
		if(m_pTransferBuffer)
		{
			SDL_ReleaseGPUTransferBuffer(m_pDevice, m_pTransferBuffer);
		}

		SDL_ReleaseWindowFromGPUDevice(m_pDevice, m_pWindow);
		SDL_DestroyGPUDevice(m_pDevice);
		Log("SDL GPU Device destroyed");
	}

	bool SDLGPUDevice::BeginFrame(const Color& ClearColor)
	{
		// Known: the picture freezes while a window border is held (Windows runs its own loop). 
		// Fix later with SDL_AddEventWatch and SDL_EVENT_WINDOW_EXPOSED when the engine owns the main loop.

		// No projection yet, so anything drawn stretches with the window until the engine has its own matrix maths

		m_pCommandBuffer = SDL_AcquireGPUCommandBuffer(m_pDevice);

		if(m_pCommandBuffer == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		SDL_GPUTexture* SwapChainTexture = nullptr;
		Uint32 Width = 0, Height = 0;

		if(!SDL_WaitAndAcquireGPUSwapchainTexture(m_pCommandBuffer, m_pWindow, &SwapChainTexture, &Width, &Height))
		{
			LogError(SDL_GetError());
			SDL_SubmitGPUCommandBuffer(m_pCommandBuffer);
			m_pCommandBuffer = nullptr;
			return false;
		}

		if(SwapChainTexture == nullptr)
		{
			SDL_SubmitGPUCommandBuffer(m_pCommandBuffer);
			m_pCommandBuffer = nullptr;
			return false;
		}

		SDL_GPUColorTargetInfo ColorTargetInfo{};
		ColorTargetInfo.clear_color = { ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A };
		ColorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		ColorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
		ColorTargetInfo.texture = SwapChainTexture;

		m_pRenderPass = SDL_BeginGPURenderPass(m_pCommandBuffer, &ColorTargetInfo, 1, nullptr);

		if(m_pRenderPass == nullptr)
		{
			LogError(SDL_GetError());
			SDL_SubmitGPUCommandBuffer(m_pCommandBuffer);
			m_pCommandBuffer = nullptr;
			return false;
		}
		return true;
	}

	void SDLGPUDevice::EndFrame()
	{
		if (m_pRenderPass == nullptr) return;

		SDL_EndGPURenderPass(m_pRenderPass);
		SDL_SubmitGPUCommandBuffer(m_pCommandBuffer);

		m_pRenderPass = nullptr;
		m_pCommandBuffer = nullptr;
	}

	void SDLGPUDevice::DrawTestTriangle()
	{
		if(m_pRenderPass == nullptr || m_pGraphicsPipeline == nullptr) return;
	
		SDL_BindGPUGraphicsPipeline(m_pRenderPass, m_pGraphicsPipeline);

		SDL_GPUBufferBinding BufferBinding{};
		BufferBinding.buffer = m_pVertexBuffer;
		BufferBinding.offset = 0;

		SDL_BindGPUVertexBuffers(m_pRenderPass, 0, &BufferBinding, 1);
		SDL_DrawGPUPrimitives(m_pRenderPass, 3, 1, 0, 0);

	}

	bool SDLGPUDevice::CreateVertexBuffer()
	{
		SDL_GPUBufferCreateInfo VertexBufferInfo{};
		VertexBufferInfo.size = sizeof(Vertices);
		VertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
		m_pVertexBuffer = SDL_CreateGPUBuffer(m_pDevice, &VertexBufferInfo);
		if(m_pVertexBuffer == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		SDL_GPUTransferBufferCreateInfo TransferInfo{};
		TransferInfo.size = sizeof(Vertices);
		TransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
		m_pTransferBuffer = SDL_CreateGPUTransferBuffer(m_pDevice, &TransferInfo);
		if (m_pTransferBuffer == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		void* Data = SDL_MapGPUTransferBuffer(m_pDevice, m_pTransferBuffer, false);
		if (Data == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}
		SDL_memcpy(Data, Vertices, sizeof(Vertices));

		SDL_UnmapGPUTransferBuffer(m_pDevice, m_pTransferBuffer);

		SDL_GPUCommandBuffer* CommandBuffer = SDL_AcquireGPUCommandBuffer(m_pDevice);
		if (CommandBuffer == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		SDL_GPUCopyPass* CopyPass = SDL_BeginGPUCopyPass(CommandBuffer);
		if (CopyPass == nullptr)
		{
			LogError(SDL_GetError());
			SDL_CancelGPUCommandBuffer(CommandBuffer);
			return false;
		}

		/// Where the data is coming from
		SDL_GPUTransferBufferLocation Location{};
		Location.transfer_buffer = m_pTransferBuffer;
		Location.offset = 0;

		/// Where the data is going
		SDL_GPUBufferRegion Region{};
		Region.buffer = m_pVertexBuffer;
		Region.size = sizeof(Vertices);
		Region.offset = 0;

		SDL_UploadToGPUBuffer(CopyPass, &Location, &Region, true);

		SDL_EndGPUCopyPass(CopyPass);

		if(!SDL_SubmitGPUCommandBuffer(CommandBuffer))
		{
			LogError(SDL_GetError());
			return false;
		}
		SDL_ReleaseGPUTransferBuffer(m_pDevice, m_pTransferBuffer);
		m_pTransferBuffer = nullptr;

		return true;
	}

	bool SDLGPUDevice::CreatePipeline()
	{
		SDL_GPUShader* VertexShader = LoadShader(m_pDevice, "Triangle.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX);
		if (VertexShader == nullptr) return false;

		SDL_GPUShader* FragmentShader = LoadShader(m_pDevice, "Triangle.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT);
		if(FragmentShader == nullptr)
		{
			SDL_ReleaseGPUShader(m_pDevice, VertexShader);
			return false;
		}

		SDL_GPUVertexBufferDescription VertexBufferDescription{};
		VertexBufferDescription.slot = 0;
		VertexBufferDescription.pitch = static_cast<Uint32>(sizeof(Vertex));
		VertexBufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
		VertexBufferDescription.instance_step_rate = 0;

		SDL_GPUVertexAttribute VertexAttributes[2]{};
		VertexAttributes[0].location = 0;
		VertexAttributes[0].buffer_slot = 0;
		VertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
		VertexAttributes[0].offset = static_cast<Uint32>(offsetof(Vertex, X));
		VertexAttributes[1].location = 1;
		VertexAttributes[1].buffer_slot = 0;
		VertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
		VertexAttributes[1].offset = static_cast<Uint32>(offsetof(Vertex, R));

		SDL_GPUColorTargetDescription ColorTargetDescription{};
		ColorTargetDescription.format = SDL_GetGPUSwapchainTextureFormat(m_pDevice, m_pWindow);

		SDL_GPUGraphicsPipelineCreateInfo PipelineInfo{};
		PipelineInfo.vertex_shader = VertexShader;
		PipelineInfo.fragment_shader = FragmentShader;
		PipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		PipelineInfo.vertex_input_state.vertex_buffer_descriptions = &VertexBufferDescription;
		PipelineInfo.vertex_input_state.num_vertex_buffers = 1;
		PipelineInfo.vertex_input_state.vertex_attributes = VertexAttributes;
		PipelineInfo.vertex_input_state.num_vertex_attributes = 2;
		PipelineInfo.target_info.color_target_descriptions = &ColorTargetDescription;
		PipelineInfo.target_info.num_color_targets = 1;

		m_pGraphicsPipeline = SDL_CreateGPUGraphicsPipeline(m_pDevice, &PipelineInfo);

		SDL_ReleaseGPUShader(m_pDevice, VertexShader);
		SDL_ReleaseGPUShader(m_pDevice, FragmentShader);

		if(m_pGraphicsPipeline == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		return true;
	}

}
