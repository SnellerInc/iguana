#!/usr/bin/env bash

# A configuration helper that would create a build directory and
# call cmake to configure builds for the following configurations:
#
#   build/Debug - Debug build
#   build/Release - Release build
#
# This structure is compatible with many build systems out of box.

CURRENT_DIR="`pwd`"
BUILD_DIR="${CURRENT_DIR}/build"
BUILD_OPTIONS="-DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DIGUANA_TEST=1"

echo "== [Iguana Build Configuration - Debug] =="
eval cmake "${CURRENT_DIR}" -B "${BUILD_DIR}/Debug" -DCMAKE_BUILD_TYPE=Debug ${BUILD_OPTIONS}
echo ""

echo "== [Iguana Build Configuration - Release] =="
eval cmake "${CURRENT_DIR}" -B "${BUILD_DIR}/Release" -DCMAKE_BUILD_TYPE=Release ${BUILD_OPTIONS}
echo ""
