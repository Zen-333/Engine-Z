cmake_minimum_required(VERSION 3.25)  # Oldest CMake allowed to read this
project(EngineZ LANGUAGES CXX)        # Names the project

set(CMAKE_CXX_STANDARD 20)            # Use C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)   # Fail instead of quietly falling back
set(CMAKE_CXX_EXTENSIONS OFF)         # Standard c++ only

add_subdirectory(Engine)              # Go read Engine/CMakeLists.txt
add_subdirectory(Games/Sandbox)       # Go read the game's recipe