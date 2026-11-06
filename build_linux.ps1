# The server for Linux (the deployed one), built here in a Docker container of the
# server's own system (Ubuntu 26.04: its glibc), from src\server\CMakeLists.txt.
# Output src\build\linux\tictactoe_server (stripped; libstdc++ inside).
#   powershell -ExecutionPolicy Bypass -File build_linux.ps1
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$image = 'kazilib-linux-build:26.04'

docker image inspect $image *> $null
if ($LASTEXITCODE) {
	# The compiler image, made once
	"FROM ubuntu:26.04`nRUN apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq g++ cmake ninja-build >/dev/null && rm -rf /var/lib/apt/lists/*`n" |
		docker build -q -t $image - | Out-Null
	if ($LASTEXITCODE) { throw "docker build of $image failed" }
}

$jobs = [Math]::Max(1, [Environment]::ProcessorCount - 1)
$src = ($root -replace '\\', '/')
docker run --rm -v "${src}:/ttt" $image bash -c "set -e; cmake -S /ttt/src/server -B /tmp/b -G Ninja -DCMAKE_BUILD_TYPE=Release >/dev/null; cmake --build /tmp/b -j $jobs; strip /tmp/b/tictactoe_server; mkdir -p /ttt/src/build/linux; cp /tmp/b/tictactoe_server /ttt/src/build/linux/"
if ($LASTEXITCODE) { throw 'Linux server build failed' }
Write-Host "Linux server: $(Join-Path $root 'src\build\linux\tictactoe_server')"
