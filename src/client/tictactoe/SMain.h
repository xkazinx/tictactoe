#pragma once
#include "Scene.h"

class SMain : public Scene
{
public:
	SMain();
	~SMain();
	void initialize();
	void display() const;
};

