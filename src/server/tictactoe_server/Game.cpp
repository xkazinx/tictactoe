#include "stdafx.h"
#include "Game.h"
#include "Player.h"

Game::Game()
{
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			_pos[x][y] = nullptr;
		}
	}
}


Game::~Game()
{
}


Player * Game::victory()
{
	struct 
	{
		int x[3];
		int y[3];
	} pos[] = {
		{
			{ 0, 1, 2 },
			{ 0, 0, 0 }
		},
		{
			{ 0, 1, 2 },
			{ 1, 1, 1 }
		},
		{
			{ 0, 1, 2 },
			{ 2, 2, 2 }
		},
		{
			{ 0, 0, 0 },
			{ 0, 1, 2 }
		},
		{
			{ 1, 1, 1 },
			{ 0, 1, 2 }
		},
		{
			{ 2, 2, 2 },
			{ 0, 1, 2 }
		},
		{
			{ 0, 1, 2 },
			{ 0, 1, 2 }
		},
		{
			{ 2, 1, 0 },
			{ 0, 1, 2 }
		},
	};

	for (auto & p : pos)
	{
		bool is = _pos[p.x[0]][p.y[0]] == _pos[p.x[1]][p.y[1]] && _pos[p.x[0]][p.y[0]] == _pos[p.x[2]][p.y[2]] && _pos[p.x[0]][p.y[0]] != nullptr;
		if (is)
		{
			return _pos[p.x[0]][p.y[0]];
		}
	}

	return nullptr;
}

bool Game::ended()
{
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			if (_pos[x][y] == nullptr)
				return false;
		}
	}

	return true;
}