# testSdlImguiLinux

## Visual Studio 2022 (CMake)
Open folder in Visual Studio.
Presets are in CMakePresets.json.

Demo: OFF (set by script flag -demo).

## Build from CLI
cmake --preset vs2022-x64
cmake --build --preset vs2022-x64-release

## Docker (Linux, build-only)
docker build -t testSdlImguiLinux .