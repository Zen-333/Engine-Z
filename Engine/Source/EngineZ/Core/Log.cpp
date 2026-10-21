#include <EngineZ/Core/Log.h>
#include <SDL3/SDL_version.h>
#include <iostream>

namespace EngineZ
{
	void Log(std::string_view Message)
	{
		const int Version = SDL_GetVersion();
		std::cout << "[Engine Z] Log: " << Message << '\n';
	}
	void LogError(std::string_view ErrorMessage)
	{
		std::cout << "[Engine Z] ERROR: " << ErrorMessage << '\n';
	}
}