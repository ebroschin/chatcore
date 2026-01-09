#!/usr/bin/env bash
set -e

VCPKG_ROOT="$(cd "$(dirname "$0")/../external/vcpkg" && pwd)"

if [ ! -f "$VCPKG_ROOT/vcpkg" ]; then
  "$VCPKG_ROOT/bootstrap-vcpkg.sh"
fi


