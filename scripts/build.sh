#!/usr/bin/env bash
set -e

bash -x scripts/bootstrap_vcpkg.sh --disable-metrics

cmake -S . -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=external/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_C_COMPILER=clang-20 \
  -DCMAKE_CXX_COMPILER=clang++-20

cmake --build build
