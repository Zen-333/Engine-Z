#include <EngineZ/Renderer/Renderer.h>
#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>

namespace EngineZ
{
	Renderer::Renderer(Window& MainWindow) : m_pDevice(CreateGraphicsDevice(MainWindow))
	{

	}

	Renderer::~Renderer()
	{

	}

	bool Renderer::BeginFrame(const Color& ClearColor)
	{
		if(m_pDevice)
		{
			return m_pDevice->BeginFrame(ClearColor);
		}
		return false;
	}

	void Renderer::EndFrame()
	{
		if(m_pDevice)
		{
			m_pDevice->EndFrame();
		}
	}

	void Renderer::DrawTestTriangle() 
	{
		if(m_pDevice)
		{
			m_pDevice->DrawTestTriangle();
		}
	}

	bool Renderer::IsValid() const
	{
		if (m_pDevice == nullptr) return false;
		return true;
	}
}