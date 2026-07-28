#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

bash -x "${SCRIPT_DIR}/bootstrap_vcpkg.sh" -disableMetrics

cd ${REPO_ROOT}
cmake --preset linux-clang-release-client
cmake --build "${REPO_ROOT}/build/linux-clang-release-client"

cmake --preset linux-clang-release-load-tester
cmake --build "${REPO_ROOT}/build/linux-clang-release-load-tester"

cmake --preset linux-clang-release-server
cmake --build "${REPO_ROOT}/build/linux-clang-release-server"
