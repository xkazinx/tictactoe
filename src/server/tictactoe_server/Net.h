#pragma once
class Net
{
public:
	Socket::Handler _sock;
	void tick();
	void init();
};

extern Net g_net;