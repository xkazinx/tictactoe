#pragma once
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
#endif
#include <cstdint>
using u64 = std::uint64_t;
using i64 = std::int64_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u16 = std::uint16_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;
using i8 = std::int8_t;

// winsock2 before anything includes <windows.h> (initflags.h does), or windows.h
// brings the old winsock.h and enet's winsock2.h redefines it all. Linux (the
// server) and the browser (the client, Emscripten) have neither.
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#endif

// kazilib: std names without std::, startup flags, enet and its packet layer
#include <stdnames.h>
#include <initflags.h>
#include <under.h>
#include <text/utf8.h>
#include <net/packet.h>
#include <net/socket.h>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>

using lib::OPkt;
using lib::OPktPtr;
using lib::IPkt;
using lib::PktCap;
using lib::Under;
using lib::EnumCStr;
using magic_enum::enum_name;
using magic_enum::enum_cast;
using magic_enum::enum_values;
using magic_enum::enum_count;
using Socket = lib::Socket;
using SocketHandler = lib::SocketHandler;

// The server's address: launch flags ip=<host> port=<n>, or init.json next to the exe
inline constexpr const char * kDefaultIp = "127.0.0.1";
inline constexpr u16 kDefaultPort = 6987;
inline string ServerIp() { return lib::init::Value("ip", kDefaultIp); }
inline u16 ServerPort() { return (u16)lib::init::Number("port", kDefaultPort); }

#define stdremove(list, func) list.erase(std::remove_if(list.begin(), list.end(), func), list.end());

// Every packet starts with one: u8 on the wire, values only appended
enum class Msg : u8
{
	Auth,			// c->s name; s->c name (accepted)
	Players,		// s->c u16 count, then per player: name, bool in_game
	PlayerExists,	// s->c the name is taken
	SelectPlayer,	// c->s opponent's name
	StartGame,		// s->c name1, turn1, name2, turn2
	Position,		// c->s int x, int y; s->c name1, turn1, name2, turn2, 9 cells
	PlayerInGame,	// s->c the chosen player is in a game
	Winner,			// s->c winner's name
	PlayAgain,		// c->s wants a rematch; s->c name of who asked (to the other)
	GoLobby,		// c->s / s->c back to the lobby
	GameEnded,		// s->c a draw
	PlayerGone,		// s->c the chosen player left before the game started
	OpponentLeft,	// s->c opponent's name: left the game (lobby or disconnect)
};

// A nickname's longest, in bytes (the entry stops there, the server cuts there)
inline constexpr size_t kNameMax = 32;
