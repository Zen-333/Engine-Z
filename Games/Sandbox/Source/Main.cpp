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
		return 1;
	}

	while(!MainWindow.ShouldClose())
	{
		MainWindow.PollEvents();
		if(!MainRenderer.BeginFrame({ .R = 11.0f / 255.0f, .G = 111.0f / 255.0f, .B = 1.0f, .A = 1.0f }))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
		MainRenderer.DrawTestTriangle();
		MainRenderer.EndFrame();
	}
}
