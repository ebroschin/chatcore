$ErrorActionPreference = "Stop"

$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$REPO_ROOT = Resolve-Path (Join-Path $SCRIPT_DIR "..\..") | Select-Object -ExpandProperty Path

& (Join-Path $SCRIPT_DIR "bootstrap_vcpkg.ps1") -disableMetrics

Set-Location $REPO_ROOT

cmake --preset linux-debug-client
cmake --build "$REPO_ROOT\build\linux-debug-client"

cmake --preset linux-debug-load-tester
cmake --build "$REPO_ROOT\build\linux-debug-load-tester"

cmake --preset linux-debug-server
cmake --build "$REPO_ROOT\build\linux-debug-server"