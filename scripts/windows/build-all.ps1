param(
    [string]$PresetPrefix = "windows-msvc-release"
)

$ErrorActionPreference = "Stop"

$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$REPO_ROOT = Resolve-Path (Join-Path $SCRIPT_DIR "..\..") | Select-Object -ExpandProperty Path

& (Join-Path $SCRIPT_DIR "bootstrap_vcpkg.ps1") -disableMetrics

Set-Location $REPO_ROOT

foreach ($app in @("client", "load-tester", "server")) {
    $preset = "$PresetPrefix-$app"
    cmake --preset $preset
    cmake --build "$REPO_ROOT\build\$preset"
}
