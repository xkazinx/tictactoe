# to-resolve.md (tictactoe)

## Web client and Linux server (2026-11-06, uncommitted; continue.md)

Manual checks:

- **Firefox and Safari**: open https://kazin.dev/projects/tictactoe/, log in, play a game
  against a second tab. Why: only Edge (Chromium, software WebGL) was tested; Safari's
  WebGL2 and its keyboard events differ most.
- **A phone**: same page. Why: SFGUI's widgets want a mouse and a keyboard; a touch
  arrives as SFML's first-finger mouse events, and the on-screen keyboard is untested.
- **From outside the home network** (mobile data): the page and a game. Why: the tests ran
  on the LAN (dnsmasq resolves kazin.dev to the box).
- **Leave a tab in the background a minute**, come back, play on. Why: browsers pause a
  hidden tab's frames; the WebSocket keeps the ENet peer alive through the gateway, but
  the client handles the queued packets only when the tab is visible again.

Open questions:

- **The background picture's license**: `art\background_1920x1080.jpg` ("01918_midastouch",
  a wallpaper from the Desktop) is now public on kazin.dev. Is it free to publish, or
  should it be replaced?

- **Standardize?** Use kazilib's web port (SFML on SDL3, the GL ES 1 emulation, the
  gateway) for the Helbreath clients? The Helbreath client also needs `sf::Shader` (no
  shaders in SFML's ES path), audio, and its Windows-only code behind switches.
- **Native clients on the live server?** Open UDP 6987 on the box's firewall (ufw) and the
  router, or leave the live server browser-only.
