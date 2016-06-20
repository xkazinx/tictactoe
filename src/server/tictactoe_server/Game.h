#pragma once
class Player;
class Game
{
public:
	Game();
	~Game();

	Player * _pos[3][3];
	Player * victory();
	bool ended();
};

