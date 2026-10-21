#pragma once
#include <string_view>

namespace EngineZ 
{
	void Log(std::string_view Message);
	void LogError(std::string_view ErrorMessage);
}