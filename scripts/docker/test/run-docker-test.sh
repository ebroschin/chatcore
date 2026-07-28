#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../../.." && pwd)"
IMAGE_TAG="${IMAGE_TAG:-chatcore-demo-local}"
DOCKERFILE="${SCRIPT_DIR}/Dockerfile.linux.test"

cd "${REPO_ROOT}"

echo "Building ${IMAGE_TAG} from local tree (context: ${REPO_ROOT})"
DOCKER_BUILDKIT=1 docker build \
  -t "${IMAGE_TAG}" \
  -f "${DOCKERFILE}" \
  .

echo "Running demo..."
docker run --rm -it -p 1338:1338 "${IMAGE_TAG}" \
  bash -lc '/workspace/src/scripts/docker/demo.sh'
