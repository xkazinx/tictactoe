#!/usr/bin/env bash
# Installs tictactoe on the Ubuntu box from an uploaded folder (deploy_ubuntu.ps1
# sends it to /tmp/tictactoe-upload): the server under /opt/tictactoe/bin as the
# service tictactoe (system user tictactoe), the web build under /opt/tictactoe/web,
# Caddy's snippet in /etc/caddy/sites/kazin.dev.d/ (Caddy restarted only when the
# snippet changed: its admin API is off).
#   sudo bash install.sh [<upload folder>]
set -euo pipefail
SRC="${1:-/tmp/tictactoe-upload}"
SNIPPET=/etc/caddy/sites/kazin.dev.d/tictactoe.caddy

id tictactoe >/dev/null 2>&1 || useradd --system --no-create-home --shell /usr/sbin/nologin tictactoe

install -d -m 755 /opt/tictactoe /opt/tictactoe/bin
install -m 755 "$SRC/tictactoe_server" /opt/tictactoe/bin/tictactoe_server.new
mv -f /opt/tictactoe/bin/tictactoe_server.new /opt/tictactoe/bin/tictactoe_server

# The web build swapped whole (a visitor never gets half of two builds)
rm -rf /opt/tictactoe/web.new
cp -r "$SRC/web" /opt/tictactoe/web.new
chmod -R a+rX /opt/tictactoe/web.new
rm -rf /opt/tictactoe/web.old
[ -d /opt/tictactoe/web ] && mv /opt/tictactoe/web /opt/tictactoe/web.old
mv /opt/tictactoe/web.new /opt/tictactoe/web
rm -rf /opt/tictactoe/web.old

install -m 644 "$SRC/tictactoe.service" /etc/systemd/system/tictactoe.service
systemctl daemon-reload
systemctl enable tictactoe >/dev/null 2>&1
systemctl restart tictactoe

if ! cmp -s "$SRC/tictactoe.caddy" "$SNIPPET"; then
	[ -f "$SNIPPET" ] && cp "$SNIPPET" "$SNIPPET.bak"
	install -m 644 "$SRC/tictactoe.caddy" "$SNIPPET"
	if ! caddy validate --config /etc/caddy/Caddyfile --adapter caddyfile >/tmp/tictactoe-caddy.log 2>&1; then
		echo "(!) Caddy refused the snippet:"; tail -5 /tmp/tictactoe-caddy.log
		if [ -f "$SNIPPET.bak" ]; then mv -f "$SNIPPET.bak" "$SNIPPET"; else rm -f "$SNIPPET"; fi
		exit 1
	fi
	rm -f "$SNIPPET.bak"
	systemctl restart caddy
	echo "Caddy: snippet installed, restarted"
fi

sleep 1
systemctl is-active tictactoe
journalctl -u tictactoe -n 5 --no-pager -o cat
