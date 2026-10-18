#pragma once
class Net
{
public:
	unique_ptr<SocketHandler> _sock;
	void tick();
	void init();
};

extern Net g_net;
