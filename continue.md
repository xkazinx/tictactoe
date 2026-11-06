# continue.md (tictactoe, master)

## Repos

| Repo | Commit | State |
|---|---|---|
| tictactoe | master: the web / Linux / deploy commit (this file's) on `0f61330` (the struct cleanup) | pushed |
| kazilib (`src/dep/kazilib`) | master `2a274a9` (web builds, the gateway, portable initflags) | pushed |

The kazilib pointer moved from `a137487` to master: the client needed `SMain` on kazilib's
`lib::Initializes` (`_init` now means "set up ran": the old meaning added empty windows)
and `Testerino.cpp` on `tcore::Endpoint::_dir`.

## Last work (2026-11-06)

- **Web client:** the same client in a browser (Emscripten). SFML 3.1 with a window
  backend on SDL3 and OpenGL ES 1 emulated on WebGL2; SFGUI's shader renderer on
  WebGL2; ENet's API on a WebSocket (`enet/websocket.cpp`). `build_web.ps1`.
- **Server:** a WebSocket gateway thread (kazilib `net/ws_gateway.h`) passes browsers
  to the server as ENet peers on loopback; the same exe serves native and web
  clients (`ws=`, `ws_bind=`, `web=`).
- **Linux:** the server builds for Ubuntu 26.04 in Docker (`build_linux.ps1`).
- **Background picture:** `art\background_1920x1080.jpg` (from the user's Desktop),
  cropped to `deploy\sprites\background.jpg` (800x600), drawn behind every window
  (native and web, `SMain`: scaled to cover).
- **Showcase:** canvas shots of a live game in `docs\showcase\` (`1_login` ..
  `5_lost.png`); as 1024x768 JPEGs on kazin.dev/projects (portfolio repo
  `img/portfolio/projects/tictactoe*.jpg`, the hover panel of another session, entry
  `tictactoe` with `photos`; uncommitted there).
- **Deployed:** https://kazin.dev/projects/tictactoe/ (`deploy_ubuntu.ps1`): service
  `tictactoe` on `kazin-music`, Caddy snippet `kazin.dev.d/tictactoe.caddy`.

## Tested

| Test | Result |
|---|---|
| Testerino `login` / `lobby_widgets` / `game` / `disconnect` (native, Windows) | 74 / 37 / 184 / 72 passed, 0 failed |
| `run_protocol_test.cmd` | passed, 0 failed |
| `src\tests\web\run_web_test.ps1` (two Edge tabs, local Windows server) | passed |
| same, `-Url https://kazin.dev/projects/tictactoe/` (Caddy, wss, Linux server) | passed |
| native client against a browser client, one server (by hand with `tt.js`) | a whole game, "WebA won the game!" on the native side |
| kazilib `web/smoke` (SFML drawing on the web port) from fresh clones (`build_deps.ps1`) | passed |

Known gaps: only Chromium (Edge, SwiftShader) tested; no Firefox / Safari, no phone,
no real GPU; native clients can't reach the live server (UDP 6987 closed by ufw).

## Next steps

- Decide whether to standardize (Helbreath): the web port's limits there are in
  kazilib `web/README.md` (no `sf::Shader`, no audio module built, one window).
- Commit / push when asked: kazilib branch `web` first, then tictactoe with the pointer.
- `to-resolve.md`: the manual checks.

## The user's prompts (this work)

1. "https://github.com/xkazinx/tictactoe this is at code, I want you to try to make it work for a web browser, for this, do it entirely within the context of tictactoe and based on the result we see if we standarize, you can still modify kazilib anyway, for example try to port sfml and sfgui to emscripsten or somehow to make it work at web browser and for enet do it so that you implement a websocket inside with a thread which then translates the message to enet internally and fast so that the same exe can use websockets"
2. "you can make a custom sfml build if needed or other things"
3. "when it's all done upload it to ubuntu server kazin.dev/projects/tictactoe"
4. "why sdl?"
5. "see if you can build the server for linux and make it work in ubuntu as well, build it here and send to ubuntu server and make it run as a service"
6. "use more agents if apply"
