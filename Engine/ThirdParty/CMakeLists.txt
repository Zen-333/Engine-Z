set(SDL_STATIC ON CACHE BOOL "" FORCE)           # Build SDL as a static library
set(SDL_SHARED OFF CACHE BOOL "" FORCE)	         # Don’t build SDL3.dll
set(SDL_TESTS  OFF CACHE BOOL "" FORCE)          # Don’t build SDL’s test program
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)    # Don’t build SDL’s test helper library
set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)  # Don’t generate install rules

add_subdirectory(SDL3 EXCLUDE_FROM_ALL)          # Prevents building unneeded targets included