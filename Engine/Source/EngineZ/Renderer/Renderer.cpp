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

}