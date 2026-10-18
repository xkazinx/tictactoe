#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>
#include <cstdint>
#pragma comment(lib, "winmm.lib")

// The frame clock: milliseconds from timeGetTime, seeded once per frame
class Time {
public:
	using Val = std::uint32_t;

private:
	Val _cached_time = 0;
	Val _delta = 0;
	float _deltaf = 0.f;

public:
	Time() {
		_cached_time = timeGetTime();
	}

	void seed() {
		const Val now = timeGetTime();
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
