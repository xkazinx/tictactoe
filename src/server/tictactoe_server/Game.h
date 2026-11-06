#pragma once
struct Player;
struct Game
{
	Game();
	~Game();

	Player * _pos[3][3];
	Player * victory();
	bool ended();
};

