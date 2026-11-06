// Tic Tac Toe - SFML.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "App.h"
App g_app;

#include "Driver.h"
Driver g_wnd;

#include "GUI.h"
GUI g_ui;

#include "Scene.h"
Scenes g_scene; // kazilib's scene manager and the scenes (destroyed before g_ui)

#include <ctime.h>
Time Clock;

#include "Net.h"
Net g_net;

#include "SMain.h"
SMain * s_main;

#include "Player.h"
Player g_player;

int main()
{
	try
	{
		g_app.init();
	}
	catch (const std::exception & e)
	{
		std::cerr << "(!) exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

