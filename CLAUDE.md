# CLAUDE.md (tictactoe)

`D:\code\CLAUDE.md` applies here too ("upd", commit messages, builds). This project:

- **Build:** `powershell -ExecutionPolicy Bypass -File build.ps1`: `setup.ps1` (submodules, SFML 3.1, SFGUI 1.0.1),
  then kazilib's FASTBuild runner on `src\fastbuild.json` (kazilib, server, client): **Release, x86 (Win32), the
  64-bit-hosted tools** (`src\Directory.Build.props`), distributed to the workers that answer. Debug only when the
  prompt asks (`build.ps1 -Configuration Debug`). Aliases: `build.ps1 server`, `build.ps1 client`; `-Regen`,
  `-Clean`, `-Local`. Settings live in the `.vcxproj` files; the `.bff` is generated (`src\build\fbuild\`).
  Build after every prompt that changes code; it stops this checkout's running exes first.
- **Kazilib first:** use what kazilib has (packets, sockets, scenes, utf8, enums, Testerino, FASTBuild) before
  writing code here; code another project could use goes to kazilib.
- **Tests:** `src\tests\run_protocol_test.cmd` (headless) and the Testerino routines in `src\tests\testerino\`
  (`node src\dep\kazilib\testerino\tt.js run src\tests\testerino\<name>.tt`), all to 0 failed after a change they
  cover. Run test programs outside the command sandbox (the sandbox ends or hides the clients it starts).
- **Protocol:** `src\shared\tdef.h` `Msg` (u8) is append-only; a packet's writer and reader change together.
- **Original README stays:** README.md keeps its 2016 original word for word at its end (`## Original README`);
  changes edit only the part above it (`D:\code\CLAUDE.md`, "Original READMEs stay": a README unchanged for more
  than six months keeps its original underneath when it is rewritten).
- **Names:** members `_name`, functions and classes `PascalCase` for new code (`D:\code\hb\hbmillennium_7\CLAUDE.md`
  has the full rules every project follows).
