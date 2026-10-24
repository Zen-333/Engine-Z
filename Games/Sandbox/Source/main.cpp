#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>

#include <chrono>
#include <thread>

int main()
{

	EngineZ::Window MainWindow("Sandbox Game", 640, 480);

	while(!MainWindow.ShouldClose())
	{
		MainWindow.PollEvents();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	EngineZ::Log("Window Closed");
	
}