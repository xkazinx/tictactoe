# CLAUDE.md (tictactoe)

- **Build:** `powershell -ExecutionPolicy Bypass -File build.ps1` (`setup.ps1`: submodules, SFML 3.1, SFGUI 1.0.1; then kazilib FASTBuild on `src\fastbuild.json`: kazilib, server, client). `build.ps1 server|client`, `-Regen`, `-Clean`, `-Local`; `-Configuration Debug` only if asked. Stops this checkout's running exes first.
- **Kazilib first:** use kazilib (packets, sockets, scenes, utf8, enums, Testerino, FASTBuild); reusable code goes there.
- **Tests:** `src\tests\run_protocol_test.cmd` + `src\tests\testerino\*.tt`, 0 failed, outside the command sandbox.
- **Protocol:** `src\shared\tdef.h` `Msg` (u8) append-only; writer and reader change together.
- **README:** 2016 original stays at its end (`## Original README`).
- **Names:** members `_name`, functions / types `PascalCase` (full rules: `D:\code\hb\hbmillennium\CLAUDE.md`).
