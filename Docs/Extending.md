# Extending Engine Z

How to add code, games and libraries. 
For how the codebase is organised, see [Architecture.md](Architecture.md).

## Adding code

* **New engine source file:** create it under `Engine/Source/EngineZ/<Module>/` and add the `.cpp` file to the `add_library(EngineZ STATIC ...)` list in `Engine/CMakeLists.txt`. Headers are not listed. CMake does not scan folders, so a `.cpp` file missing from the list is silently not built.
* **New engine module:** a new folder under `Engine/Source/EngineZ/`, handled the same way as a new file.
* **New game:** create `Games/<Name>/` with a `CMakeLists.txt` (`add_executable` and `target_link_libraries(<Name> PRIVATE EngineZ)`) and a `Source/` folder, then add `add_subdirectory(Games/<Name>)` to the root `CMakeLists.txt`.

## Adding or upgrading a third party library

Libraries are vendored: copied unmodified into the repo. The policy is in the Third party libraries section of [Architecture.md](Architecture.md).

1. Download the release source for the exact version, and copy it into `Engine/ThirdParty/<Name>/` so that its own `CMakeLists.txt` sits directly inside that folder. When upgrading, replace the old folder. Do not edit the files.
2. Stage it with `git add -f Engine/ThirdParty/<Name>`. Libraries ship their own `.gitignore` files, and Git obeys those silently. For example, SDL's `src/render/gpu/shaders/.gitignore` ignores `.hlsl` files, so a plain `git add` would leave them out. The `-f` flag overrides that.
3. Compare the number of tracked files with the number of files on disk. They must match.

   ```
   git ls-files Engine/ThirdParty/<Name> | find /c /v ""
   dir /s /b /a-d Engine\ThirdParty\<Name> | find /c /v ""
   ```

4. New library only: set its options and add it with `add_subdirectory` in `Engine/ThirdParty/CMakeLists.txt`, then link it in `Engine/CMakeLists.txt` as `PRIVATE`. Use `PUBLIC` only if a public engine header needs the library's types.
5. Update the library table in [Architecture.md](Architecture.md) with the new version.