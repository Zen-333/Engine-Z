#!/bin/sh
# Engine Z architecture checks. Blocks the commit if a rule is broken.

Failed=0

Check()
{
	Description="$1"
	shift
	Output=$("$@")
	if [ -n "$Output" ]; then
		echo "RULE BROKEN: $Description"
		echo "$Output"
		echo
		Failed=1
	fi
}

Check "SDL include inside an engine header" \
	git grep --cached -n 'SDL3/' -- 'Engine/Source/*.h'

Check "SDL include outside Platform/ and Renderer/Backend/SDLGPU/" \
	git grep --cached -n 'SDL3/' -- 'Engine/Source/*' ':!Engine/Source/EngineZ/Platform/*' ':!Engine/Source/EngineZ/Renderer/Backend/SDLGPU/*'

Check "SDL_ name inside a Renderer header" \
	git grep --cached -n 'SDL_' -- 'Engine/Source/EngineZ/Renderer/*.h' ':!Engine/Source/EngineZ/Renderer/Backend/*'

Check "Renderer header includes a backend file" \
	git grep --cached -n 'Backend/' -- 'Engine/Source/EngineZ/Renderer/*.h' ':!Engine/Source/EngineZ/Renderer/Backend/*'

Check "SDL used inside Games/" \
	git grep --cached -n 'SDL' -- 'Games/*'

Check "Engine includes something from Games/" \
	git grep --cached -n -E 'include.*Games/' -- 'Engine/Source/*'

Check ".. in an include path" \
	git grep --cached -n -E '#include.*\.\.' -- 'Engine/Source/*' 'Games/*'

if [ "$Failed" -ne 0 ]; then
	echo "Commit blocked. Fix the lines above, or skip once with: git commit --no-verify"
	exit 1
fi
exit 0