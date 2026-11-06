#include "stdafx.h"
#include "App.h"
#include "Net.h"
#include "PlayerMgr.h"
#include "Testerino.h"

App g_app;
Net g_net;
PlayerMgr g_players;
Time Clock;

int main()
{
	// A line at a time also when stdout is a pipe or a file (systemd's journal)
	std::setvbuf(stdout, nullptr, _IOLBF, 1024);
	std::cout.setf(std::ios::unitbuf);

	testerino::Prepare();
	g_app.init();
	return 0;
}
