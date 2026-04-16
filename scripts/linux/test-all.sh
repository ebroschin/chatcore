#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

cd "${REPO_ROOT}"
cmake --preset linux-test-all
cmake --build "${REPO_ROOT}/build/linux-test-all"
ctest --test-dir "${REPO_ROOT}/build/linux-test-all" --output-on-failure
