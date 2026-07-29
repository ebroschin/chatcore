#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

# Shared preset prefix without the app suffix, e.g. linux-gcc-release
PRESET_PREFIX="${1:-linux-gcc-release}"

bash -x "${SCRIPT_DIR}/bootstrap_vcpkg.sh" -disableMetrics

cd "${REPO_ROOT}"

for app in client load-tester server; do
  preset="${PRESET_PREFIX}-${app}"
  cmake --preset "${preset}"
  cmake --build "${REPO_ROOT}/build/${preset}"
done
