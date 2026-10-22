#include <EngineZ/Core/Log.h>
#include <iostream>

namespace EngineZ
{
	void Log(std::string_view Message)
	{
		std::cout << "[Engine Z] Log: " << Message << '\n';
	}
	void LogError(std::string_view ErrorMessage)
	{
		std::cout << "[Engine Z] ERROR: " << ErrorMessage << '\n';
	}
}