# Builds the Linux server (build_linux.ps1) and the web client (build_web.ps1), sends
# both to the Ubuntu box with the service, Caddy snippet and installer
# (deploy\ubuntu\), and installs them there: kazin.dev/projects/tictactoe.
#   powershell -ExecutionPolicy Bypass -File deploy_ubuntu.ps1 [-Box kazin-music] [-NoBuild]
param([string]$Box = 'kazin-music', [switch]$NoBuild)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot

if (-not $NoBuild) {
	& (Join-Path $root 'build_linux.ps1')
	& (Join-Path $root 'build_web.ps1')
}

$stage = Join-Path $root 'src\build\upload'
Remove-Item -Recurse -Force $stage -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force (Join-Path $stage 'web') | Out-Null
Copy-Item (Join-Path $root 'src\build\linux\tictactoe_server') $stage
Copy-Item (Join-Path $root 'src\build\web\client-build\index.*') (Join-Path $stage 'web')
Copy-Item (Join-Path $root 'deploy\ubuntu\*') $stage

ssh $Box 'rm -rf /tmp/tictactoe-upload'
scp -q -r $stage "${Box}:/tmp/tictactoe-upload"
if ($LASTEXITCODE) { throw 'upload failed' }
ssh $Box 'sudo bash /tmp/tictactoe-upload/install.sh && rm -rf /tmp/tictactoe-upload'
if ($LASTEXITCODE) { throw 'install failed' }
Write-Host 'Deployed: https://kazin.dev/projects/tictactoe/'
