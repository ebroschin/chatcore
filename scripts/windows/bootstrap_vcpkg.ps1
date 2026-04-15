$ErrorActionPreference = "Stop"

$VCPKG_ROOT = Join-Path $PSScriptRoot "..\..\third_party\vcpkg"
$VCPKG_ROOT = (Resolve-Path $VCPKG_ROOT).Path

$vcpkgExe = Join-Path $VCPKG_ROOT "vcpkg.exe"
$bootstrapScript = Join-Path $VCPKG_ROOT "bootstrap-vcpkg.bat"

if (-not (Test-Path $vcpkgExe)) {
    & $bootstrapScript @args
}