#pragma once

struct SDL_Window;

namespace EngineZ
{
	/// Owns the operating system window and its events.
	class Window
	{
	public:

		/// @note If SDL or the window cannot be created, the error is logged, ShouldClose returns true
		/// and GetSDLWindow returns nullptr.
		Window(const char* WindowTitle, const int Width, const int Height);
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;
		~Window();

		void PollEvents();
		[[nodiscard]] bool ShouldClose() const;

		[[nodiscard]] SDL_Window* GetSDLWindow() const { return m_pWindow; }

	private:

		SDL_Window* m_pWindow = nullptr;
		bool m_bShouldClose = false;
	};
}
