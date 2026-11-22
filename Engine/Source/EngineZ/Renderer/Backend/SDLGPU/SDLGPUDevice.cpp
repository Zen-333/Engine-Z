#include <EngineZ/Renderer/Backend/SDLGPU/SDLGPUDevice.h>

#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <string>

namespace EngineZ
{
	namespace
	{
		struct Vertex
		{
			float X = 0.0f;
			float Y = 0.0f;
			float Z = 0.0f;
			float R = 0.0f;
			float G = 0.0f;
			float B = 0.0f;
			float A = 0.0f;
		};

		constexpr std::array<Vertex, 3> TRIANGLE_VERTICES
		{
			{
				{.X = 0.0f, .Y = 0.5f, .Z = 0.0f, .R = 0.0f, .G = 0.0f, .B = 1.0f, .A = 1.0f},    // Top vertex
				{.X = -0.5f, .Y = -0.5f, .Z = 0.0f, .R = 0.0f, .G = 1.0f, .B = 0.0f, .A = 1.0f},  // Bottom left vertex
				{.X = 0.5f, .Y = -0.5f, .Z = 0.0f, .R = 1.0f, .G = 0.0f, .B = 0.0f, .A = 1.0f}    // Bottom right vertex
			}
		};

		constexpr Uint32 TRIANGLE_VERTEX_COUNT = static_cast<Uint32>(TRIANGLE_VERTICES.size());
		constexpr Uint32 TRIANGLE_DATA_SIZE = static_cast<Uint32>(sizeof(TRIANGLE_VERTICES));

		struct FormatName
		{
			SDL_GPUShaderFormat Flag = SDL_GPU_SHADERFORMAT_INVALID;
			const char* Name = nullptr;
		};

		constexpr std::array<FormatName, 5> FORMAT_NAMES =
		{
			{
				{ .Flag = SDL_GPU_SHADERFORMAT_SPIRV,    .Name = "SPIR-V"   },
				{ .Flag = SDL_GPU_SHADERFORMAT_DXBC,     .Name = "DXBC"     },
				{ .Flag = SDL_GPU_SHADERFORMAT_DXIL,     .Name = "DXIL"     },
				{ .Flag = SDL_GPU_SHADERFORMAT_MSL,      .Name = "MSL"      },
				{ .Flag = SDL_GPU_SHADERFORMAT_METALLIB, .Name = "METALLIB" },
			}
		};

		std::string ShaderFormatsToString(const SDL_GPUShaderFormat Format)
		{
			std::string Result;

			for(const FormatName& Entry : FORMAT_NAMES)
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

		SDL_GPUShader* LoadShader(SDL_GPUDevice* Device, const std::string& FileName, SDL_GPUShaderStage Stage)
		{
			assert(Device != nullptr && "LoadShader needs a device");

			const char* pBasePath = SDL_GetBasePath();
			if(pBasePath == nullptr)
			{
				LogError(SDL_GetError());
				return nullptr;
			}

			const std::string Path = std::string(pBasePath) + "Shaders/SPIRV/" + FileName;

			size_t CodeSize = 0;
			void* pCode = SDL_LoadFile(Path.c_str(), &CodeSize);
			if(pCode == nullptr)
			{
				LogError(SDL_GetError());
				return nullptr;
			}

			SDL_GPUShaderCreateInfo Info{};
			Info.code = static_cast<const Uint8*>(pCode);
			Info.code_size = CodeSize;
			Info.entrypoint = "main";
			Info.format = SDL_GPU_SHADERFORMAT_SPIRV;
			Info.stage = Stage;
			SDL_GPUShader* pShader = SDL_CreateGPUShader(Device, &Info);

			SDL_free(pCode);

			if(pShader == nullptr)
			{
				LogError(SDL_GetError());
				return nullptr;
			}

			return pShader;
		}
	}


	// ---- LIFECYCLE ----

	SDLGPUDevice::SDLGPUDevice(SDL_GPUDevice* Device, SDL_Window* MainWindow)
		: m_pDevice(Device), m_pWindow(MainWindow)
	{
		assert(m_pDevice != nullptr && "SDLGPUDevice needs a device");
		assert(m_pWindow != nullptr && "SDLGPUDevice needs a window");
	}

	SDLGPUDevice::~SDLGPUDevice()
	{
		if(m_pGraphicsPipeline != nullptr)
		{
			SDL_ReleaseGPUGraphicsPipeline(m_pDevice, m_pGraphicsPipeline);
		}
		if(m_pVertexBuffer != nullptr)
		{
			SDL_ReleaseGPUBuffer(m_pDevice, m_pVertexBuffer);
		}
		if(m_pTransferBuffer != nullptr)
		{
			SDL_ReleaseGPUTransferBuffer(m_pDevice, m_pTransferBuffer);
		}

		SDL_ReleaseWindowFromGPUDevice(m_pDevice, m_pWindow);
		SDL_DestroyGPUDevice(m_pDevice);
		m_pDevice = nullptr;
		m_pGraphicsPipeline = nullptr;
		m_pVertexBuffer = nullptr;
		m_pTransferBuffer = nullptr;
		Log("SDL GPU Device destroyed");
	}

	// ---- OVERRIDES ----

	bool SDLGPUDevice::BeginFrame(const Color ClearColor)
	{
		// TODO: the picture freezes while a window border is held (Windows runs its own loop). 
		// Fix later with SDL_AddEventWatch and SDL_EVENT_WINDOW_EXPOSED when the engine owns the main loop.

		// No projection yet, so anything drawn stretches with the window until the engine has its own matrix maths.

		m_pCommandBuffer = SDL_AcquireGPUCommandBuffer(m_pDevice);

		if(m_pCommandBuffer == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		SDL_GPUTexture* pSwapChainTexture = nullptr;
		Uint32 Width = 0;
		Uint32 Height = 0;

		if(!SDL_WaitAndAcquireGPUSwapchainTexture(m_pCommandBuffer, m_pWindow, &pSwapChainTexture, &Width, &Height))
		{
			LogError(SDL_GetError());
			SDL_SubmitGPUCommandBuffer(m_pCommandBuffer);
			m_pCommandBuffer = nullptr;
			return false;
		}

		if(pSwapChainTexture == nullptr)
		{
			SDL_SubmitGPUCommandBuffer(m_pCommandBuffer);
			m_pCommandBuffer = nullptr;
			return false;
		}

		SDL_GPUColorTargetInfo ColorTargetInfo{};
		ColorTargetInfo.clear_color = { ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A };
		ColorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
		ColorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
		ColorTargetInfo.texture = pSwapChainTexture;

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
		if(m_pRenderPass == nullptr)
		{
			return;
		}

		SDL_EndGPURenderPass(m_pRenderPass);
		SDL_SubmitGPUCommandBuffer(m_pCommandBuffer);

		m_pRenderPass = nullptr;
		m_pCommandBuffer = nullptr;
	}

	void SDLGPUDevice::DrawTestTriangle()
	{
		if(m_pRenderPass == nullptr || m_pGraphicsPipeline == nullptr)
		{
			return;
		}
	
		SDL_BindGPUGraphicsPipeline(m_pRenderPass, m_pGraphicsPipeline);

		SDL_GPUBufferBinding BufferBinding{};
		BufferBinding.buffer = m_pVertexBuffer;
		BufferBinding.offset = 0;

		SDL_BindGPUVertexBuffers(m_pRenderPass, 0, &BufferBinding, 1);
		SDL_DrawGPUPrimitives(m_pRenderPass, TRIANGLE_VERTEX_COUNT, 1, 0, 0);

	}

	// ---- ACTIONS ----

	std::unique_ptr<IGraphicsDevice> SDLGPUDevice::Create(Window& MainWindow)
	{
		#ifdef NDEBUG
				const bool bIsDebugMode = false;
		#else
				const bool bIsDebugMode = true;
		#endif

		SDL_GPUDevice* pDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, bIsDebugMode, nullptr);
		if(pDevice == nullptr)
		{
			LogError(SDL_GetError());
			return nullptr;
		}

		SDL_Window* pWindowHandle = MainWindow.GetSDLWindow();
		if(!SDL_ClaimWindowForGPUDevice(pDevice, pWindowHandle))
		{
			LogError(SDL_GetError());
			SDL_DestroyGPUDevice(pDevice);
			return nullptr;
		}

		Log("SDL GPU Device created");
		Log(std::string("GPU driver: ") + SDL_GetGPUDeviceDriver(pDevice));
		Log("Shader formats: " + ShaderFormatsToString(SDL_GetGPUShaderFormats(pDevice)));

		auto pNewDevice = std::make_unique<SDLGPUDevice>(pDevice, pWindowHandle);

		if(!pNewDevice->CreateVertexBuffer())
		{
			return nullptr;
		}
		if(!pNewDevice->CreatePipeline())
		{
			return nullptr;
		}

		Log("Vertex buffer and shaders created");
		return pNewDevice;
	}

	// ---- HELPERS ----

	bool SDLGPUDevice::CreateVertexBuffer()
	{
		SDL_GPUBufferCreateInfo VertexBufferInfo{};
		VertexBufferInfo.size = TRIANGLE_DATA_SIZE;
		VertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
		m_pVertexBuffer = SDL_CreateGPUBuffer(m_pDevice, &VertexBufferInfo);
		if(m_pVertexBuffer == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		SDL_GPUTransferBufferCreateInfo TransferInfo{};
		TransferInfo.size = TRIANGLE_DATA_SIZE;
		TransferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
		m_pTransferBuffer = SDL_CreateGPUTransferBuffer(m_pDevice, &TransferInfo);
		if(m_pTransferBuffer == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		void* pData = SDL_MapGPUTransferBuffer(m_pDevice, m_pTransferBuffer, false);
		if(pData == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}
		SDL_memcpy(pData, TRIANGLE_VERTICES.data(), TRIANGLE_DATA_SIZE);

		SDL_UnmapGPUTransferBuffer(m_pDevice, m_pTransferBuffer);

		SDL_GPUCommandBuffer* pCommandBuffer = SDL_AcquireGPUCommandBuffer(m_pDevice);
		if(pCommandBuffer == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		SDL_GPUCopyPass* pCopyPass = SDL_BeginGPUCopyPass(pCommandBuffer);
		if(pCopyPass == nullptr)
		{
			LogError(SDL_GetError());
			SDL_CancelGPUCommandBuffer(pCommandBuffer);
			return false;
		}

		// Where the data is coming from
		SDL_GPUTransferBufferLocation Location{};
		Location.transfer_buffer = m_pTransferBuffer;
		Location.offset = 0;

		// Where the data is going
		SDL_GPUBufferRegion Region{};
		Region.buffer = m_pVertexBuffer;
		Region.size = TRIANGLE_DATA_SIZE;
		Region.offset = 0;

		SDL_UploadToGPUBuffer(pCopyPass, &Location, &Region, true);

		SDL_EndGPUCopyPass(pCopyPass);

		if(!SDL_SubmitGPUCommandBuffer(pCommandBuffer))
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
		SDL_GPUShader* pVertexShader = LoadShader(m_pDevice, "Triangle.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX);
		if(pVertexShader == nullptr)
		{
			return false;
		}

		SDL_GPUShader* pFragmentShader = LoadShader(m_pDevice, "Triangle.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT);
		if(pFragmentShader == nullptr)
		{
			SDL_ReleaseGPUShader(m_pDevice, pVertexShader);
			return false;
		}

		SDL_GPUVertexBufferDescription VertexBufferDescription{};
		VertexBufferDescription.slot = 0;
		VertexBufferDescription.pitch = static_cast<Uint32>(sizeof(Vertex));
		VertexBufferDescription.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
		VertexBufferDescription.instance_step_rate = 0;

		std::array<SDL_GPUVertexAttribute, 2> VertexAttributes{};
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
		PipelineInfo.vertex_shader = pVertexShader;
		PipelineInfo.fragment_shader = pFragmentShader;
		PipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		PipelineInfo.vertex_input_state.vertex_buffer_descriptions = &VertexBufferDescription;
		PipelineInfo.vertex_input_state.num_vertex_buffers = 1;
		PipelineInfo.vertex_input_state.vertex_attributes = VertexAttributes.data();
		PipelineInfo.vertex_input_state.num_vertex_attributes = static_cast<Uint32>(VertexAttributes.size());
		PipelineInfo.target_info.color_target_descriptions = &ColorTargetDescription;
		PipelineInfo.target_info.num_color_targets = 1;

		m_pGraphicsPipeline = SDL_CreateGPUGraphicsPipeline(m_pDevice, &PipelineInfo);

		SDL_ReleaseGPUShader(m_pDevice, pVertexShader);
		SDL_ReleaseGPUShader(m_pDevice, pFragmentShader);

		if(m_pGraphicsPipeline == nullptr)
		{
			LogError(SDL_GetError());
			return false;
		}

		return true;
	}
}
