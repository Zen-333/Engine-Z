add_subdirectory(ThirdParty)                            # Add third party libraries

add_library(EngineZ STATIC								# Build a bundle of compiled code
	Source/EngineZ/Core/Log.cpp  
	Source/EngineZ/Platform/Window.cpp 
	Source/EngineZ/Renderer/Backend/SDLGPU/SDLGPUDevice.cpp 
	Source/EngineZ/Renderer/IGraphicsDevice.cpp
	Source/EngineZ/Renderer/Renderer.cpp)

target_include_directories(EngineZ PUBLIC Source)       # Let #include <EngineZ/...> find our headers
target_link_libraries(EngineZ PRIVATE SDL3::SDL3)       # Engine uses SDL internally

target_compile_options(EngineZ PRIVATE                  # Strict warnings, treated as errors (not applied to SDL)
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror>)
