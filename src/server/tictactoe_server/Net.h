#pragma once
#include <net/ws_gateway.h>

struct Net
{
	unique_ptr<SocketHandler> _sock;
	// Browser clients (the web build) in through WebSockets, as ENet peers of _sock
	// (kazilib net/ws_gateway.h): launch flags ws=<port> (default the game port + 1,
	// 0 = off), ws_bind=<address> (default 127.0.0.1: a reverse proxy in front),
	// web=<folder> (the web build's files, served on the same port; default the
	// exe's web\ folder when there is one)
	lib::ws::Gateway _ws;
	void tick();
	void init();
};

extern Net g_net;
