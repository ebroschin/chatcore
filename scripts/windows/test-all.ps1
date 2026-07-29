param(
    [string]$Preset = "windows-msvc-test-all"
)

$ErrorActionPreference = "Stop"

$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$REPO_ROOT = Resolve-Path (Join-Path $SCRIPT_DIR "..\..") | Select-Object -ExpandProperty Path

Set-Location $REPO_ROOT

cmake --preset $Preset
cmake --build "$REPO_ROOT\build\$Preset"
ctest --test-dir "$REPO_ROOT\build\$Preset" -LE flaky --output-on-failure
