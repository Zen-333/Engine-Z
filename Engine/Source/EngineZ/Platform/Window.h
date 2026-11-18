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
		[[nodiscard]] bool ShouldClose() const;

		[[nodiscard]] SDL_Window* GetSDLWindow() const { return m_pWindow; }

	private:

		SDL_Window* m_pWindow = nullptr;
		bool m_bShouldClose = false;

	};

}
