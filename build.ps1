# Builds kazilib, the server (server\tictactoe_server.exe) and the client
# (deploy\tictactoe.exe): x86, with the 64-bit-hosted tools (src\Directory.Build.props).
# Release unless -Debug. Runs setup.ps1 first for any library still missing.
param([switch]$Debug)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$cfg = if ($Debug) { 'Debug' } else { 'Release' }

& (Join-Path $root 'setup.ps1')

# Stop this checkout's running exes so the link can write them
Get-Process tictactoe, tictactoe_d, tictactoe_server, tictactoe_server_d -ErrorAction SilentlyContinue |
    Where-Object { $_.Path -like "$root\*" } | Stop-Process -Force

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$msbuild = & $vswhere -latest -prerelease -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\amd64\MSBuild.exe' | Select-Object -First 1
if (-not $msbuild) { throw 'MSBuild not found' }

& $msbuild (Join-Path $root 'src\tictactoe.sln') /m /nologo /v:minimal "/p:Configuration=$cfg" /p:Platform=Win32
exit $LASTEXITCODE
