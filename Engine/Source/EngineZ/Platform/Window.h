#pragma once

struct SDL_Window;

namespace EngineZ
{
	class Window
	{
	public:

		Window(const char* WindowTitle, const int Width, const int Height);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void PollEvents();
		bool ShouldClose() const;

		inline SDL_Window* GetSDLWindow() { return m_Window; };

	private:

		SDL_Window* m_Window = nullptr;
		bool m_bCloseRequested = false;

	};

}