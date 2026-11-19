add_executable(Sandbox "Source/Main.cpp" )		# Build a program
add_custom_target(CopyShaders                   # Runs every build
	COMMAND ${CMAKE_COMMAND} -E rm -rf "$<TARGET_FILE_DIR:Sandbox>/Shaders"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/Engine/Shaders/Compiled"
	"$<TARGET_FILE_DIR:Sandbox>/Shaders"
	COMMENT "Copying shaders next to the exe")
add_dependencies(Sandbox CopyShaders)
target_link_libraries(Sandbox PRIVATE EngineZ)  # Attach the engine to it

target_compile_options(Sandbox PRIVATE          # Strict warnings, treated as errors
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror>)