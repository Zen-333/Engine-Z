#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>

#include <chrono>
#include <thread>

int main()
{

	EngineZ::Window mMainWindow("Sandbox Game", 640, 480);

	while(!mMainWindow.ShouldClose())
	{
		mMainWindow.PollEvents();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	EngineZ::Log("Window Closed");
	
}