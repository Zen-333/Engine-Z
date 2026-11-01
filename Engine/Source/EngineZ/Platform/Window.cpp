#include <EngineZ/Platform/Window.h>
#include <EngineZ/Core/Log.h>
#include <SDL3/SDL.h>


namespace EngineZ
{

	Window::Window(const char* WindowTitle, const int Width, const int Height)
	{

		if(!SDL_Init(SDL_INIT_VIDEO))
		{
			LogError(SDL_GetError());
			m_bCloseRequested = true;
			return;
		}

		SDL_WindowFlags WindowFlags = SDL_WINDOW_RESIZABLE;
		m_Window = SDL_CreateWindow(WindowTitle, Width, Height, WindowFlags);
		if(m_Window == nullptr)
		{
			LogError(SDL_GetError());
			m_bCloseRequested = true;
			return;
		}

		Log("SDL Window created");

	}

	Window::~Window()
	{
		if(m_Window != nullptr)
		{
			SDL_DestroyWindow(m_Window);
		}
		SDL_Quit();

		Log("SDL Window destroyed");
	}

	void Window::PollEvents()
	{
		SDL_Event Event;
		while(SDL_PollEvent(&Event))
		{
			if(Event.type == SDL_EVENT_QUIT )
			{
				m_bCloseRequested = true;
			}
			else if(Event.type == SDL_EVENT_KEY_DOWN && Event.key.key == SDLK_ESCAPE)
			{
				m_bCloseRequested = true;
			}
		}
	
	}

	bool Window::ShouldClose() const
	{
		return m_bCloseRequested;
	}

}
