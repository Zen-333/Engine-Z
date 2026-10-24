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
			return;
		}

		mWindow = SDL_CreateWindow(WindowTitle, Width, Height, SDL_WINDOW_OPENGL);
		if(mWindow == nullptr)
		{
			LogError(SDL_GetError());
			mbCloseRequested = true;
		}
	}

	Window::~Window()
	{
		if(mWindow != nullptr)
		{
			SDL_DestroyWindow(mWindow);
		}
		SDL_Quit();
	}

	void Window::PollEvents()
	{
		SDL_Event Event;
		while(SDL_PollEvent(&Event))
		{
			if(Event.type == SDL_EVENT_QUIT )
			{
				mbCloseRequested = true;
			}
			else if(Event.type == SDL_EVENT_KEY_DOWN && Event.key.key == SDLK_ESCAPE)
			{
				mbCloseRequested = true;
			}
		}
	
	}

	bool Window::ShouldClose() const
	{
		return mbCloseRequested;
	}

}
