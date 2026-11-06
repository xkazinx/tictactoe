# The web client end to end: the Windows server (its WebSocket gateway serving
# server\web\, build_web.ps1) on ports 6992 / 6993, two headless browser tabs
# (kazilib web\tools\browser.js: Edge, WebGL2 on SwiftShader) logging in, one picking
# the other, a whole game to X's win. Screenshots in testerino\web\.
# Exit code = failed steps.
#   powershell -ExecutionPolicy Bypass -File src\tests\web\run_web_test.ps1 [-Url https://kazin.dev/projects/tictactoe/]
# -Url  a deployed page instead (no server started)
param([string]$Url = '')
$ErrorActionPreference = 'Stop'
$root = Resolve-Path (Join-Path $PSScriptRoot '..\..\..')
$shots = Join-Path $root 'testerino\web'
New-Item -ItemType Directory -Force $shots | Out-Null

$server = $null
if (-not $Url) {
	if (-not (Test-Path (Join-Path $root 'server\web\index.wasm'))) { throw 'no server\web\index.wasm: run build_web.ps1' }
	$server = Start-Process -FilePath (Join-Path $root 'server\tictactoe_server.exe') -ArgumentList 'port=6992', 'ws=6993' `
		-WorkingDirectory (Join-Path $root 'server') -WindowStyle Hidden -PassThru
	Start-Sleep -Milliseconds 800
	$Url = 'http://127.0.0.1:6993/index.html'
}

# Screenshot names in the steps land in testerino\web\
$steps = Join-Path $shots 'two_players.json'
(Get-Content (Join-Path $PSScriptRoot 'two_players.json') -Raw) -replace '"shot": "', ('"shot": "' + ($shots -replace '\\', '/') + '/') |
	Set-Content -Encoding utf8 $steps

try {
	node (Join-Path $root 'src\dep\kazilib\web\tools\browser.js') --url $Url --pages 2 --steps $steps
	$failed = $LASTEXITCODE
} finally {
	if ($server) { Stop-Process -Id $server.Id -Force -ErrorAction SilentlyContinue }
}
exit $failed
