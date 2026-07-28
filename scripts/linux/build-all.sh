#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

bash -x "${SCRIPT_DIR}/bootstrap_vcpkg.sh" -disableMetrics

cd ${REPO_ROOT}
cmake --preset linux-gcc-release-client
cmake --build "${REPO_ROOT}/build/linux-gcc-release-client"

cmake --preset linux-gcc-release-load-tester
cmake --build "${REPO_ROOT}/build/linux-gcc-release-load-tester"

cmake --preset linux-gcc-release-server
cmake --build "${REPO_ROOT}/build/linux-gcc-release-server"
