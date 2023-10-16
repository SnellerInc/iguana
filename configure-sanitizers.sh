#!/usr/bin/env bash

# A configuration helper that would create a build directory and
# call cmake to configure builds for the following configurations:
#
#   build/Release_ASAN - Address sanitizer enabled build
#   build/Release_UBSAN - Undefined behavior sanitizer enabled build

CURRENT_DIR="`pwd`"
BUILD_DIR="${CURRENT_DIR}/build"
BUILD_OPTIONS="-DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DIGUANA_TEST=1"

echo "== [Iguana Build Configuration - Release_ASAN] =="
eval cmake "${CURRENT_DIR}" -B "${BUILD_DIR}/Release_ASAN" ${BUILD_OPTIONS} -DCMAKE_BUILD_TYPE=Release -DIGUANA_SANITIZE=address
echo ""

echo "== [Iguana Build Configuration - Release_UBSAN] =="
eval cmake "${CURRENT_DIR}" -B "${BUILD_DIR}/Release_UBSAN" ${BUILD_OPTIONS} -DCMAKE_BUILD_TYPE=Release -DIGUANA_SANITIZE=undefined
echo ""
