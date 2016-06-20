#include "stdafx.h"
#include "App.h"
#include "Net.h"
#include "PlayerMgr.h"

App g_app;
Net g_net;
PlayerMgr g_players;
Time Clock;

int _tmain(int argc, _TCHAR* argv[])
{
	g_app.init();
	return 0;
}

