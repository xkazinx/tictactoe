#pragma once
// The server's Testerino bridge (kazilib testerino/tserver.h): on with the launch
// flag `testerino`, only in a source checkout (src\tictactoe.sln beside server\).
// Endpoint <checkout>\testerino\server\.
namespace testerino
{
	void Prepare();	// first thing in main: a stale ready removed
	void Start();	// once listening
	void Poll();	// every main loop pass
	void Stop();	// after the main loop
	// A boot that ends before listening: `failed` in ready's place when asked for
	void Failed(const string & reason);
}
