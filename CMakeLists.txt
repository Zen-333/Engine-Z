cmake_minimum_required(VERSION 3.25)  # oldest CMake allowed to read this
project(EngineZ LANGUAGES CXX)		  # names the project

set(CMAKE_CXX_STANDARD 20)            # use C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)   # fail instead of quietly falling back
set(CMAKE_CXX_EXTENSIONS OFF)         # standard c++ only

add_subdirectory(Engine)              # go read Engine/CMakeLists.txt
add_subdirectory(Games/Sandbox)       # go read the game's recipe