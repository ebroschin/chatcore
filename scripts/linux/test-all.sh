#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

PRESET="${1:-linux-clang-test-all}"

cd "${REPO_ROOT}"
cmake --preset "${PRESET}"
cmake --build "${REPO_ROOT}/build/${PRESET}"
ctest --test-dir "${REPO_ROOT}/build/${PRESET}" -LE flaky --output-on-failure
