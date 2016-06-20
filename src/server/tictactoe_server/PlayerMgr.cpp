#include "stdafx.h"
#include "PlayerMgr.h"
#include "Player.h"

PlayerMgr::~PlayerMgr()
{
	for (auto p : _list)
		delete p;

	_list.clear();
}

Player * PlayerMgr::find(ENetPeer * peer)
{
	for (auto p : _list)
	{
		if (p->_peer == peer)
			return p;
	}

	return nullptr;
}

Player * PlayerMgr::find(string & name)
{
	for (auto p : _list)
	{
		if (p->_name == name)
			return p;
	}

	return nullptr;
}


void PlayerMgr::add(ENetPeer * peer)
{
	auto p = new Player;
	p->_peer = peer;
	_list.emplace_back(p);
}

void PlayerMgr::remove(ENetPeer * peer)
{
	stdremove(_list, [&](Player * p)
	{
		if (p->_peer == peer)
		{
			delete p;
			return true;
		}
		return false;
	});
}