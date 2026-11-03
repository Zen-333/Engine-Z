#include <EngineZ/Renderer/Renderer.h>
#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>

namespace EngineZ
{
	Renderer::Renderer(Window& InWindow) : m_Device(CreateGraphicsDevice(InWindow))
	{

	}

	Renderer::~Renderer()
	{

	}

	bool Renderer::BeginFrame(const Color& ClearColor)
	{
		if(m_Device)
		{
			return m_Device->BeginFrame(ClearColor);
		}
		return false;
	}

	void Renderer::EndFrame()
	{
		if(m_Device)
		{
			m_Device->EndFrame();
		}
	}

}