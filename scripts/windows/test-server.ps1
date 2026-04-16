$ErrorActionPreference = "Stop"

$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$REPO_ROOT = Resolve-Path (Join-Path $SCRIPT_DIR "..\..") | Select-Object -ExpandProperty Path

Set-Location $REPO_ROOT

cmake --preset windows-test-server
cmake --build "$REPO_ROOT\build\windows-test-server" --target chatcore_server_tests
ctest --test-dir "$REPO_ROOT\build\windows-test-server" --output-on-failure
