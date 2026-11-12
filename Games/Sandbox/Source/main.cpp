#include <EngineZ/Core/Log.h>
#include <EngineZ/Platform/Window.h>
#include <EngineZ/Renderer/Renderer.h>

#include <chrono>
#include <thread>

int main()
{
	#ifdef NDEBUG
		EngineZ::Log("Build: Release");
	#else
		EngineZ::Log("Build: Debug");
	#endif

	EngineZ::Window MainWindow("Sandbox Game", 640, 480);
	EngineZ::Renderer MainRenderer(MainWindow);

	if(!MainRenderer.IsValid())
	{
		EngineZ::LogError("Renderer failed to start");
		return 1;
	}

	while(!MainWindow.ShouldClose())
	{
		MainWindow.PollEvents();
		if(!MainRenderer.BeginFrame({ 111.0f/255.0f,111.0f/255.0f,1.0f,1.0f }))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
		MainRenderer.DrawTestTriangle();
		MainRenderer.EndFrame();
		
	}
	
}