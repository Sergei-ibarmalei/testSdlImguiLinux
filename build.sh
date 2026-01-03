#!/usr/bin/env bash

# README
# chmod +x build.sh
# ./build.sh debug
# ./build.sh release

set -euo pipefail

MODE="${1:-debug}"          # debug | release
GEN="${2:-}"                # optional: ninja | make

# Choose generator
if [[ -z "${GEN}" ]]; then
  if command -v ninja >/dev/null 2>&1; then
    GEN="ninja"
  else
    GEN="make"
  fi
fi

if [[ "${MODE}" == "debug" ]]; then
  BUILD_TYPE="Debug"
  BUILD_DIR="build"
elif [[ "${MODE}" == "release" ]]; then
  BUILD_TYPE="Release"
  BUILD_DIR="build-release"
else
  echo "Usage: $0 {debug|release} [ninja|make]"
  exit 1
fi

if [[ "${GEN}" == "ninja" ]]; then
  CMAKE_GEN="-G Ninja"
else
  CMAKE_GEN=""
fi

cmake -S . -B "${BUILD_DIR}" ${CMAKE_GEN} -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
cmake --build "${BUILD_DIR}" -j
echo "Built: ${BUILD_DIR}"


