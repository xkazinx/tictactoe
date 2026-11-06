# CLAUDE.md (tictactoe)

- **Build:** `powershell -ExecutionPolicy Bypass -File build.ps1` (`setup.ps1`: submodules, SFML 3.1, SFGUI 1.0.1; then kazilib FASTBuild on `src\fastbuild.json`: kazilib, server, client). `build.ps1 server|client`, `-Regen`, `-Clean`, `-Local`; `-Configuration Debug` only if asked. Stops this checkout's running exes first. `setup.ps1`'s `submodule update` resets kazilib to the recorded pointer: stage a moved pointer first (`git add src/dep/kazilib`).
- **Web:** `build_web.ps1` (Emscripten, kazilib `web\`: SFML / SFGUI patches, `kazilib_web.cmake`, `shell.html`) → `server\web\`; `src\client\CMakeLists.txt` is the web build only. Code that differs: `#ifdef __EMSCRIPTEN__` (the loop, the WebSocket URL), Windows-only: `#ifdef _WIN32` (Testerino bridges, background window).
- **Linux / deploy:** `build_linux.ps1` (Docker, Ubuntu 26.04, `src\server\CMakeLists.txt`); `deploy_ubuntu.ps1` → box `kazin-music`: service `tictactoe`, `/opt/tictactoe`, Caddy `kazin.dev/projects/tictactoe` (`deploy\ubuntu\`).
- **Kazilib first:** use kazilib (packets, sockets, scenes, utf8, enums, Testerino, FASTBuild, WebSocket gateway); reusable code goes there.
- **Tests:** `src\tests\run_protocol_test.cmd` + `src\tests\testerino\*.tt` + `src\tests\web\run_web_test.ps1`, 0 failed, outside the command sandbox.
- **Protocol:** `src\shared\tdef.h` `Msg` (u8) append-only; writer and reader change together.
- **README:** 2016 original stays at its end (`## Original README`).
- **Names:** members `_name`, functions / types `PascalCase` (full rules: `D:\code\hb\hbmillennium\CLAUDE.md`).
