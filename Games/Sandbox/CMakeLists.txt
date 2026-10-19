add_executable(Sandbox Source/main.cpp "Source/Text.h")		    # build a program
target_link_libraries(Sandbox PRIVATE EngineZ)  # attach the engine to it