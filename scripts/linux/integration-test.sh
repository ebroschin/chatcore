#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

PRESET_PREFIX="${PRESET_PREFIX:-linux-gcc-debug-thread}"
PORT="${PORT:-18338}"
CLIENTS="${CLIENTS:-8}"
READY_TIMEOUT_SEC="${READY_TIMEOUT_SEC:-60}"

SERVER_BIN="${REPO_ROOT}/build/${PRESET_PREFIX}-server/apps/server/chatcore-server"
LOAD_BIN="${REPO_ROOT}/build/${PRESET_PREFIX}-load-tester/apps/load-tester/chatcore-load-tester"

SERVER_LOG="$(mktemp)"
LOAD_LOG="$(mktemp)"
DB_PATH="$(mktemp)"
SERVER_PID=""

cleanup() {
  local exit_code=$?
  if [[ -n "${SERVER_PID}" ]] && kill -0 "${SERVER_PID}" 2>/dev/null; then
    kill "${SERVER_PID}" 2>/dev/null || true
    wait "${SERVER_PID}" 2>/dev/null || true
  fi
  rm -f "${DB_PATH}" "${SERVER_LOG}" "${LOAD_LOG}"
  exit "${exit_code}"
}
trap cleanup EXIT

if [[ ! -x "${SERVER_BIN}" ]]; then
  echo "missing server binary: ${SERVER_BIN}" >&2
  echo "build first, e.g.: cmake --preset ${PRESET_PREFIX}-server && cmake --build ${REPO_ROOT}/build/${PRESET_PREFIX}-server" >&2
  exit 1
fi
if [[ ! -x "${LOAD_BIN}" ]]; then
  echo "missing load-tester binary: ${LOAD_BIN}" >&2
  echo "build first, e.g.: cmake --preset ${PRESET_PREFIX}-load-tester && cmake --build ${REPO_ROOT}/build/${PRESET_PREFIX}-load-tester" >&2
  exit 1
fi

"${SERVER_BIN}" \
  --ip 127.0.0.1 \
  --port "${PORT}" \
  --db "${DB_PATH}" \
  --log info \
  >"${SERVER_LOG}" 2>&1 &
SERVER_PID=$!

for ((i = 0; i < READY_TIMEOUT_SEC * 2; i++)); do
  if ! kill -0 "${SERVER_PID}" 2>/dev/null; then
    echo "server exited before becoming ready" >&2
    cat "${SERVER_LOG}" >&2
    exit 1
  fi
  if (echo >/dev/tcp/127.0.0.1/"${PORT}") 2>/dev/null; then
    break
  fi
  if ((i == READY_TIMEOUT_SEC * 2 - 1)); then
    echo "timed out waiting for server on port ${PORT}" >&2
    cat "${SERVER_LOG}" >&2
    exit 1
  fi
  sleep 0.5
done

set +e
"${LOAD_BIN}" \
  --ip 127.0.0.1 \
  --port "${PORT}" \
  --clients "${CLIENTS}" \
  --log info \
  >"${LOAD_LOG}" 2>&1
LOAD_RC=$?
set -e

set +e
kill "${SERVER_PID}" 2>/dev/null
wait "${SERVER_PID}" 2>/dev/null
SERVER_RC=$?
set -e
SERVER_PID=""

echo "===== load-tester log ====="
cat "${LOAD_LOG}"
echo "===== server log ====="
cat "${SERVER_LOG}"

if grep -q "ThreadSanitizer:" "${LOAD_LOG}" "${SERVER_LOG}"; then
  echo "ThreadSanitizer reported an issue" >&2
  exit 1
fi

if [[ "${LOAD_RC}" -ne 0 ]]; then
  echo "load-tester failed with exit code ${LOAD_RC}" >&2
  exit "${LOAD_RC}"
fi

# 143 = 128 + SIGTERM(15): expected after we kill the server
if [[ "${SERVER_RC}" -ne 0 && "${SERVER_RC}" -ne 143 ]]; then
  echo "server failed with exit code ${SERVER_RC}" >&2
  exit "${SERVER_RC}"
fi

echo "integration test ok"
