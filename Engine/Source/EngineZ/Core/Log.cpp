#include <EngineZ/Core/Log.h>
#include "../../Games/Sandbox/Source/Text.h"
#include <iostream>

namespace EngineZ
{
	void Log(std::string_view Message)
	{
		std::cout << "[Engine Z] " << Message << '\n';
	}
}