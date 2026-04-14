#!/usr/bin/env bash
set -e

VCPKG_ROOT="$(cd "$(dirname "$0")/../../third_party/vcpkg" && pwd)"

if [ ! -f "$VCPKG_ROOT/vcpkg" ]; then
  "$VCPKG_ROOT/bootstrap-vcpkg.sh" "$@"
fi
