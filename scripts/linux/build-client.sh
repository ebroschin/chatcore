#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

bash -x "${SCRIPT_DIR}/bootstrap_vcpkg.sh" -disableMetrics

cd ${REPO_ROOT}
cmake --preset linux-gcc-debug-client
cmake --build "${REPO_ROOT}/build/linux-gcc-debug-client"
