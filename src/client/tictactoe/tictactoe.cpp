// Tic Tac Toe - SFML.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "App.h"
App g_app;

#include "Driver.h"
Driver g_wnd;

int _tmain(int argc, _TCHAR* argv[])
{
	g_app.init();
	return 0;
}

