#pragma once

namespace EngineZ 
{
	struct Color { float R; float G; float B; float A; };

	class GraphicsDevice
	{
	
	public:

		virtual ~GraphicsDevice() = default;

		virtual bool BeginFrame(const Color& ClearColor) = 0;
		virtual void EndFrame() = 0;

	};
}
