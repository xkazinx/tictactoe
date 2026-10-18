@echo off
rem Builds protocol_test.exe (x86, 64-bit-hosted tools) against the built kazilib,
rem starts the server, runs the test, stops the server. Exit code = failures.
setlocal enabledelayedexpansion
set ROOT=%~dp0..\..
rem !...!: the ")" of ProgramFiles(x86) would close the for's parentheses
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set VSDIR=
for /f "delims=" %%i in ('"!VSWHERE!" -latest -prerelease -property installationPath') do set "VSDIR=%%i"
if not defined VSDIR (echo Visual Studio not found & exit /b 100)
call "%VSDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64_x86 >nul || exit /b 100
set OUT=%ROOT%\src\build\tests
if not exist "%OUT%" mkdir "%OUT%"
cl /nologo /std:c++latest /EHsc /MD /O2 /utf-8 /DNOMINMAX /D_WINSOCK_DEPRECATED_NO_WARNINGS ^
  /I"%ROOT%\src\shared" /I"%ROOT%\src\dep\kazilib\lib\shared\include" /I"%ROOT%\src\dep\kazilib\enet\include" ^
  "%~dp0protocol_test.cpp" /Fo"%OUT%\\" /Fe"%OUT%\protocol_test.exe" ^
  /link "%ROOT%\src\dep\kazilib\bin\kazilib-ml-Release-Win32.lib" ws2_32.lib winmm.lib >"%OUT%\build.txt" || (type "%OUT%\build.txt" & exit /b 101)
start "" /min /d "%ROOT%\server" "%ROOT%\server\tictactoe_server.exe"
ping -n 2 127.0.0.1 >nul
"%OUT%\protocol_test.exe"
set RC=%ERRORLEVEL%
rem Stops only this checkout's server
powershell -NoProfile -Command "$s = (Resolve-Path '%ROOT%\server').Path; Get-Process tictactoe_server -ErrorAction SilentlyContinue | Where-Object { $_.Path -like ($s + '\*') } | Stop-Process -Force"
exit /b %RC%
