#pragma once

struct Net
{
	Net();
	~Net();
	void init();
	void tick();
	void msg_process(IPkt * in);
	void connected();
	void not_auth(IPkt * in);
	void not_players(IPkt * in);
	void Send(const OPktPtr & out);

	unique_ptr<SocketHandler> _sock;
};

extern Net g_net;
