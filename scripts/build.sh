#!/usr/bin/env bash
set -e

bash ./scripts/bootstrap_vcpkg.sh

cmake -S . -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=external/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build build
