#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

cd "${REPO_ROOT}"
cmake --preset linux-clang-test-all
cmake --build "${REPO_ROOT}/build/linux-clang-test-all"
ctest --test-dir "${REPO_ROOT}/build/linux-clang-test-all" -LE flaky --output-on-failure
