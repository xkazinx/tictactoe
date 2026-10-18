#pragma once
// The client's Testerino bridge (kazilib testerino/tcore.h): on with the launch flag
// testerino=<slot> (1..9), only in a source checkout (src\tictactoe.sln beside
// deploy\). Endpoint <checkout>\testerino\client_<slot>\.
namespace testerino
{
	void Start();
	void Pump();						// once per frame, before drawing
	void AfterDraw(sf::RenderWindow & wnd);	// drawn, not presented yet: screenshots
	void Stop();
	bool Active();
}
