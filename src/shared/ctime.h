#pragma once
#include <cstdint>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#else
#include <chrono>
#endif

// The frame clock: milliseconds (timeGetTime on Windows, the steady clock on Linux
// and in a browser), seeded once per frame
struct Time {
	using Val = std::uint32_t;

	Val _cached_time = 0;
	Val _delta = 0;
	float _deltaf = 0.f;

	static Val Now() {
#ifdef _WIN32
		return timeGetTime();
#else
		return (Val)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
#endif
	}

	Time() {
		_cached_time = Now();
	}

	void seed() {
		const Val now = Now();
		_delta = now - _cached_time; // unsigned: wraps correctly past 49.7 days
		_deltaf = _delta / 1000.f;
		_cached_time = now;
	}

	Val now() const { return _cached_time; }
	Val delta() const { return _delta; }
	float deltaf() const { return _deltaf; }

	Val elapsed(const Val t) const { return _cached_time - t; }
	bool elapsed(const Val t, const Val period) const { return elapsed(t) >= period; }

	// True once period passed since t, which then moves to now
	bool elapsed(Val & t, const Val period, const bool upd) {
		const bool res = elapsed(t) >= period;
		if (res && upd)
			t = now();
		return res;
	}
};
