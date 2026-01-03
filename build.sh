#!/usr/bin/env bash

# README
# chmod +x build.sh
# ./build.sh debug
# ./build.sh release

set -euo pipefail

# Usage:
#   ./build.sh debug
#   ./build.sh release
#   ./build.sh release clang
#   ./build.sh debug gcc ninja
#
# Args:
#   1: mode     = debug | release        (default: debug)
#   2: compiler = gcc | clang | system   (default: system)
#   3: buildsys = ninja | make | auto    (default: auto)
#
# Notes:
# - Compiler is chosen at configure time. Use separate build dirs per compiler.
# - Script auto-chooses Ninja if installed, otherwise Makefiles.

MODE="${1:-debug}"
COMPILER="${2:-system}"
BUILDSYS="${3:-auto}"

case "${MODE}" in
  debug)
    BUILD_TYPE="Debug"
    ;;
  release)
    BUILD_TYPE="Release"
    ;;
  *)
    echo "Unknown mode: ${MODE}. Use: debug | release"
    exit 1
    ;;
esac

# Choose build system
if [[ "${BUILDSYS}" == "auto" ]]; then
  if command -v ninja >/dev/null 2>&1; then
    BUILDSYS="ninja"
  else
    BUILDSYS="make"
  fi
fi

CMAKE_GEN=""
if [[ "${BUILDSYS}" == "ninja" ]]; then
  CMAKE_GEN="-G Ninja"
elif [[ "${BUILDSYS}" == "make" ]]; then
  CMAKE_GEN="" # default generator is usually Unix Makefiles
else
  echo "Unknown build system: ${BUILDSYS}. Use: ninja | make | auto"
  exit 1
fi

# Choose compiler
CC_CMD=""
CXX_CMD=""
COMP_TAG="system"
if [[ "${COMPILER}" == "gcc" ]]; then
  CC_CMD="gcc"
  CXX_CMD="g++"
  COMP_TAG="gcc"
elif [[ "${COMPILER}" == "clang" ]]; then
  CC_CMD="clang"
  CXX_CMD="clang++"
  COMP_TAG="clang"
elif [[ "${COMPILER}" == "system" ]]; then
  COMP_TAG="system"
else
  echo "Unknown compiler: ${COMPILER}. Use: gcc | clang | system"
  exit 1
fi

# Separate build dirs so compilers/configs don't fight each other
BUILD_DIR="build-${MODE}-${COMP_TAG}"

echo "== Configure =="
echo "Mode      : ${MODE} (${BUILD_TYPE})"
echo "Compiler  : ${COMPILER}"
echo "Build sys : ${BUILDSYS}"
echo "Build dir : ${BUILD_DIR}"
echo

# Configure
if [[ "${COMPILER}" == "system" ]]; then
  cmake -S . -B "${BUILD_DIR}" ${CMAKE_GEN} -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
else
  CC="${CC_CMD}" CXX="${CXX_CMD}" cmake -S . -B "${BUILD_DIR}" ${CMAKE_GEN} -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
fi

echo
echo "== Build =="
cmake --build "${BUILD_DIR}" -j

echo
echo "== Result =="
echo "Binary should be in: ${BUILD_DIR}/"
echo "Check compiler used:"
echo "  cat ${BUILD_DIR}/CMakeCache.txt | grep CMAKE_CXX_COMPILER"




