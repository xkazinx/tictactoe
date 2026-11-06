#pragma once

struct Player;
struct PlayerMgr
{
	~PlayerMgr();
	vector<Player*> _list;
	Player * find(ENetPeer * peer);
	Player * find(const string & name);
	void add(ENetPeer * peer);
	void remove(ENetPeer * peer);
};

extern PlayerMgr g_players;