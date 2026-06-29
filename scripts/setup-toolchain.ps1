param(
    [switch]$Force,
    [string]$Qt,
    [string]$Ninja,
    [string]$Output
)

$ErrorActionPreference = "Stop"
$cmakeArguments = @()

if ($Force) { $cmakeArguments += "-DFORCE=ON" }
if ($Qt) { $cmakeArguments += "-DQT_ROOT=$Qt" }
if ($Ninja) { $cmakeArguments += "-DNINJA_PATH=$Ninja" }
if ($Output) { $cmakeArguments += "-DOUTPUT=$Output" }

$script = Join-Path $PSScriptRoot "setup-toolchain.cmake"
& cmake @cmakeArguments -P $script
exit $LASTEXITCODE
