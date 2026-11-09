add_executable(Sandbox Source/main.cpp )		# Build a program
add_custom_target(CopyShaders                   # Runs every build
	COMMAND ${CMAKE_COMMAND} -E rm -rf "$<TARGET_FILE_DIR:Sandbox>/Shaders"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/Engine/Shaders/Compiled"
	"$<TARGET_FILE_DIR:Sandbox>/Shaders"
	COMMENT "Copying shaders next to the exe")
add_dependencies(Sandbox CopyShaders)
target_link_libraries(Sandbox PRIVATE EngineZ)  # Attach the engine to it