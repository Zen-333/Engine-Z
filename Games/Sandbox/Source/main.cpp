#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>
#include <EngineZ/Renderer/Renderer.h>

#include <chrono>
#include <thread>

int main()
{

	EngineZ::Window MainWindow("Sandbox Game", 640, 480);
	EngineZ::Renderer MainRenderer(EngineZ::GPU_DEVICE_SDL, &MainWindow);

	while(!MainWindow.ShouldClose())
	{
		MainWindow.PollEvents();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	EngineZ::Log("Window Closed");
	
}