#pragma once

class Player;
class PlayerMgr
{
public:
	~PlayerMgr();
	Vector<Player*> _list;
	Player * find(ENetPeer * peer);
	Player * find(string & name);
	void add(ENetPeer * peer);
	void remove(ENetPeer * peer);
};

extern PlayerMgr g_players;