$ErrorActionPreference = "Stop"

$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$REPO_ROOT = Resolve-Path (Join-Path $SCRIPT_DIR "..\..") | Select-Object -ExpandProperty Path

& (Join-Path $SCRIPT_DIR "bootstrap_vcpkg.ps1") -disableMetrics

Set-Location $REPO_ROOT

cmake --preset windows-release-client
cmake --build "$REPO_ROOT\build\windows-release-client"

cmake --preset windows-release-load-tester
cmake --build "$REPO_ROOT\build\windows-release-load-tester"

cmake --preset windows-release-server
cmake --build "$REPO_ROOT\build\windows-release-server"