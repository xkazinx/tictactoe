# The libraries the client and server build against, x86 (Win32):
#   kazilib  src\dep\kazilib  (submodule: enet, the packet layer, std names, startup flags)
#   SFML     src\dep\SFML-3.1.0  (a junction to D:\code\kazilib\SFML-3.1.0 when that
#            shared SDK is there, as the Helbreath repos use; else the official
#            vc17 32-bit release, downloaded)
#   SFGUI    src\dep\SFGUI (submodule, tag 1.0.1), built with CMake against that SFML
#            and installed into src\dep\SFGUI-1.0.1
# Every build uses the 64-bit-hosted tools (host=x64) and the v145 toolset.
# -Force rebuilds SFGUI.
param([switch]$Force)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$dep = Join-Path $root 'src\dep'

git -C $root submodule update --init src/dep/kazilib src/dep/SFGUI
if ($LASTEXITCODE) { throw 'submodule update failed' }

# SFML 3.1.0
$sfml = Join-Path $dep 'SFML-3.1.0'
if (-not (Test-Path (Join-Path $sfml 'include\SFML\Graphics.hpp'))) {
    $shared = 'D:\code\kazilib\SFML-3.1.0'
    if (Test-Path (Join-Path $shared 'include\SFML\Graphics.hpp')) {
        New-Item -ItemType Junction -Path $sfml -Target $shared | Out-Null
        Write-Host "SFML: linked to $shared"
    } else {
        $zip = Join-Path $env:TEMP 'SFML-3.1.0-windows-vc17-32-bit.zip'
        Write-Host 'SFML: downloading 3.1.0 (vc17 32-bit)'
        Invoke-WebRequest 'https://github.com/SFML/SFML/releases/download/3.1.0/SFML-3.1.0-windows-vc17-32-bit.zip' -OutFile $zip
        Expand-Archive $zip -DestinationPath $dep -Force
        Remove-Item $zip
    }
}

# SFGUI 1.0.1
$sfgui = Join-Path $dep 'SFGUI-1.0.1'
if ($Force -or -not (Test-Path (Join-Path $sfgui 'lib\SFGUI.lib'))) {
    $build = Join-Path $root 'src\build\sfgui'
    $sfmlCmake = (Join-Path $sfml 'lib\cmake\SFML') -replace '\\', '/'
    cmake -S (Join-Path $dep 'SFGUI') -B $build -G 'Visual Studio 18 2026' -A Win32 -T 'v145,host=x64' `
        "-DSFML_DIR=$sfmlCmake" -DSFGUI_BUILD_EXAMPLES=OFF -DSFGUI_BUILD_DOC=OFF -DBUILD_SHARED_LIBS=ON `
        "-DCMAKE_INSTALL_PREFIX=$($sfgui -replace '\\', '/')"
    if ($LASTEXITCODE) { throw 'SFGUI configure failed' }
    cmake --build $build --config Release --parallel
    if ($LASTEXITCODE) { throw 'SFGUI build failed' }
    cmake --install $build --config Release
    if ($LASTEXITCODE) { throw 'SFGUI install failed' }
}

Write-Host 'Dependencies ready'
