# SDL 3.4.18 (release-3.4.18), vendored in SDL3/
# https://github.com/libsdl-org/SDL/tree/release-3.4.18

set(SDL_STATIC ON CACHE BOOL "" FORCE)         # Build SDL as a static library
set(SDL_SHARED OFF CACHE BOOL "" FORCE)	       # Don't build SDL3.dll
set(SDL_TESTS  OFF CACHE BOOL "" FORCE)        # Don't build SDL's test program
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)  # Don't build SDL's test helper library
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)       # Don't generate install rules

add_subdirectory(SDL3 EXCLUDE_FROM_ALL)        # Skip SDL targets we don't use