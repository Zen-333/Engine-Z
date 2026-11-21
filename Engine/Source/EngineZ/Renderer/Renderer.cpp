#include <EngineZ/Renderer/Renderer.h>

namespace EngineZ
{
	Renderer::Renderer(Window& MainWindow) : m_pDevice(CreateGraphicsDevice(MainWindow))
	{
	}

	bool Renderer::IsValid() const
	{
		return m_pDevice != nullptr;
	}

	bool Renderer::BeginFrame(const Color ClearColor)
	{
		if(m_pDevice == nullptr)
		{
			return false;
		}

		return m_pDevice->BeginFrame(ClearColor);
	}

	void Renderer::EndFrame()
	{
		if(m_pDevice != nullptr)
		{
			m_pDevice->EndFrame();
		}
	}

	void Renderer::DrawTestTriangle()
	{
		if(m_pDevice != nullptr)
		{
			m_pDevice->DrawTestTriangle();
		}
	}
}
