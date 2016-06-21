#pragma once

class InPacket;
class Net
{
public:
	Net();
	~Net();
	void init();
	void tick();
	void msg_process(InPacket * in);
	void connected();
	void not_auth(InPacket * in);
	void not_players(InPacket * in);
	void send(OutPacket * out);

	Socket::Handler _sock;
};

extern Net g_net;