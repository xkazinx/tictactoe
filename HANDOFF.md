# Handoff: tictactoe on kazilib, Testerino in kazilib

Session `f53602db-034e-43aa-a854-78b1d77740d6` (Claude Code, started in `D:\code\hb\hbmillennium_7`), 2026-10-18.
Read this, then each repo's CLAUDE.md (hbmillennium's rules apply to every project:
Release only, names `_member` / `PascalCase`, `g_` globals, kazilib first, commit message format).

## What the user asked, in order

1. Clone `https://github.com/xkazinx/tictactoe` into `D:\code`, make it work with the newest libraries using
   kazilib, download the others, fix it, build Release with the 64-bit-hosted compiler producing x86. **Done.**
2. "print some code while you do it" (show code snippets while working). Keep doing that.
3. "build for release win32 and open", "open another client". **Done.**
4. "audit the app fix it while also fixing all their options". **Done** (below).
5. "import the needed things from hbmillennium of testerino so that kazilib has testerino features so that this app
   and others can use the testing strategies routines and communication with processes like a base framework in
   kazilib that the projects use". **Done** (below).
6. "prioritize kazilib code over project code". **Done** for tictactoe (scene manager, utf8, enums, Testerino).
7. **OPEN:** "exception, with the menu to select debugger, do it so that testerino handles this in all projects".
   See "Next: exceptions" below.
8. "prepare md for another account to continue and open txt with session name": this file.
9. "push all worked on repos", "add a claude.md in D:\code folder which explains what upd is for next projects",
   "push the tictactoe to github with first commit name "AI CODE" ...". **Done.**
10. A FASTBuild tool in kazilib for every project (`kazilib\fastbuild\`: `fbuild_gen.ps1` generates the `.bff` from
    the `.vcxproj` files through MSBuild's evaluation, `build.ps1` runs it); tictactoe builds with it
    (`src\fastbuild.json`); every CLAUDE.md: Release, x86, 64-bit-hosted tools, Debug only when asked. **Done and
    pushed** ("push so fastbuild works everywhere with the new tools and new projects use it"): kazilib `7c5cb40`,
    hb_lib `6d87c5f`, hbmillennium `cf40dc67`, hbempire `739f65b2`, tictactoe (this commit).

## State of each repo (all pushed, "push all worked on repos")

| Repo | Pushed |
|---|---|
| kazilib (`github.com/xkazinx/kazilib`) | `cb44521` Testerino moved in (core 18, tt.js v4), `61671f0` tcore ExeDir/ExePath fix (core 19), rebased on `3807d39` |
| hb_lib (`github.com/xkazinx/hb_lib`) | `f3af7db` core removed, `testerino/tt.js` forwarder, README / CLAUDE.md |
| hbmillennium (`D:\code\hb\hbmillennium_7`) | `81c4a733` CLAUDE.md (Testerino now kazilib's), kazilib + hb_lib pointers |
| hbempire (`D:\code\hb\hbempire_7`) | `2f37ee2e` the same, rebased on another session's `ab3cf512` (its CLAUDE.md conflict merged: upstream's file plus this session's four lines) |
| tictactoe (`github.com/xkazinx/tictactoe`) | the commit "AI CODE": all of the work below, submodules kazilib (`61671f0`) and SFGUI (tag 1.0.1) |

`D:\code\CLAUDE.md` (not in any repo) explains "upd" and the commit message format for every project in D:\code.

Builds verified after the last change: tictactoe Release (`build.ps1`), hbmillennium_7 (`src\build.ps1`),
hbempire_7 (FBuild `-j3 server-Release client-Release`: its normal `build.ps1` ran out of memory for PCH,
"C3859", with about 3.5 GB of virtual memory free on this PC. Use `-j3`).

## tictactoe (D:\code\tictactoe)

- Libraries: kazilib submodule (`ml` version via `kazilib.vcxproj`, `src\Directory.Build.props`), SFML 3.1.0 x86
  (junction `src\dep\SFML-3.1.0` -> `D:\code\kazilib\SFML-3.1.0`, else downloaded by `setup.ps1`), SFGUI 1.0.1
  submodule built by CMake into `src\dep\SFGUI-1.0.1`. v145, C++latest, `/MD`, `PreferredToolArchitecture x64`.
- `build.ps1` runs `setup.ps1` (submodules, SFML, SFGUI), stops this checkout's exes, builds `src\tictactoe.sln`
  Release|Win32. **Careful:** `setup.ps1` runs `git submodule update`, which resets kazilib to the *recorded*
  pointer: stage a new kazilib commit (`git add src/dep/kazilib`) before building.
- Output: `server\tictactoe_server.exe`, `deploy\tictactoe.exe` (+ SFML / SFGUI DLLs, copied with `xcopy /D`).
- Launch flags (kazilib `initflags.h`): `ip=`, `port=` (default 127.0.0.1:6987), `title=`, `background`,
  `testerino=<slot>` (client) / `testerino` (server).
- Protocol (`src\shared\tdef.h`, `Msg` u8, append-only): `Players` now carries `name, bool in_game` per player;
  `PlayerGone`, `OpponentLeft` appended; `PlayAgain` s->c names who asked.
- Audit fixes: render loop (black frames), Msg Ok stuck when a packet arrived, Winner window over the new board,
  picking a gone player hung, opponent leaving silent, Enter/Escape keys, name cap 32 (`kNameMax`), cursor after a
  refused name, lobby (who you are, busy players, empty text), rematch notice, draw text, centred messages,
  server name cleaning (kazilib `utf8::Printable` / `FitBytes`), server bound checks, opponent links on disconnect,
  `#define cout` removed, `Scenes` on kazilib `scene/manager.h`.
- Tests (all passed at the end): `src\tests\run_protocol_test.cmd` (22 checks, headless) and Testerino routines
  `src\tests\testerino\{login,lobby_widgets,game,disconnect}.tt` (74 / 37 / 184 / 72), patterns `lib\server.tt`
  (port 6990), `lib\client.tt` (`SLOT`, `NAME`). Run: `node src\dep\kazilib\testerino\tt.js run src\tests\testerino\game.tt`.
- Client bridge `src\client\tictactoe\Testerino.cpp`: verbs `state`, `widgets`, `press <id|text>`, `click`, `type`,
  `key`, `shot`, `close`, `quit`; conds `window= msg~ end~ board= turn= name= players~`. Widgets inside the lobby's
  ScrolledWindow need the viewport offset (done). Synthetic keys set both `code` and `scancode` (SFGUI reads scancode).
- Server bridge `src\server\tictactoe_server\Testerino.cpp` on `tserver.h`: `players`, `board`, `kick`, `quit`.

## Testerino in kazilib

- `lib/shared/include/testerino/tcore.h`, `tserver.h`; `testerino/tt.js`, `testerino/README.md`. hb_lib keeps
  `testerino/tplayers.h` and a forwarder `testerino/tt.js` (old path keeps working).
- tt.js v4: `findCheckout` skips folders whose `testerino\` holds `tt.js` / `tcore.h` (the libraries themselves).
- core 19: `ExeDir` / `ExePath` via `GetModuleFileNameW` (`_get_pgmptr` is null under `wmain`: 0xC0000409).

## Pitfalls met here

- **Run test programs outside the command sandbox** (`dangerouslyDisableSandbox`): inside it, launched clients
  ended early or could not be inspected, and their exe paths were unreadable to `tt.js`.
- Dead test processes (0 threads) can keep `deploy\tictactoe.exe` locked: rename it aside
  (`tictactoe.exe.old*`, git-ignored) and rebuild.
- `vcvarsall.bat` prints "'vswhere.exe' is not recognized": harmless.

## Next: exceptions (the open request)

The user saw an unhandled exception with Windows' "select a debugger" (JIT) dialog: two of the three clients the
session reopened for them (no testerino, pids 3820 / 14240, server 9364 and client 19656 still running) ended.
No WER event (JIT took it), cause unknown. Plan agreed in the session (not started, nothing edited yet):

1. **kazilib, every project:** when a program opens a Testerino endpoint (`tcore::Endpoint::Open`, so also
   `ServerBridge::Start`), install kazilib's `lib::exc::Install(true)` (`lib/shared/include/exception_report.h`):
   report `exceptions\exception_<pid>.txt` + `.dmp` next to the exe, then the process ends (no JIT dialog).
   Add a hook to `exception_report.h` (e.g. `inline void (*g_on_report)(const char * txt, DWORD code)`) called
   after `Write`, which tcore sets to: append `{"id":"event","type":"exception","code":...,"report":...}` to
   out.txt and write the `failed` file (reason `exception 0x... <name>`, `log=` the report's first stack lines)
   so `tt.js` (`start`, `send`, routines) reports "exception, report <path>" at once instead of "not running".
   Teach `tt.js` to print the failed reason / report path when a pid ends mid-routine. Bump TCORE_VERSION 20,
   tt.js VERSION 5, kazilib README / CLAUDE.md, build hbmillennium_7 and hbempire_7 (they already install
   lib::exc themselves in dev; check the order so the bridge's install does not undo a reporter they need).
2. **tictactoe:** call `lib::exc::Install(lib::exc::CatchArg())` first thing in both `_tmain`s (a normal run
   writes the report then hands on to JIT; `catchexceptions` ends), and pass `catchexceptions` in `lib\client.tt`
   / `lib\server.tt`. Make sure `.pdb`s sit beside the exes (Release has `/DEBUG`).
3. **Find the crash:** reproduce what the user did with three clients (likely candidates: closing a window with
   its X (`close` verb exits the program, check for an exception at shutdown: static destruction order of
   `g_scene` / `g_ui` / `g_wnd` / `g_net` in `tictactoe.cpp`), three-way lobby clicks, a client dropped mid-game).
   Read the newest `deploy\exceptions\exception_*.txt`, fix, add a routine for it, rerun all routines (0 failed).
4. Report to the user; commit and push in the D:\code\CLAUDE.md format (kazilib first, then the projects).
