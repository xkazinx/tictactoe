#pragma once
#include <time.h>
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <Mmsystem.h>

typedef unsigned long u32;

#ifdef max
#undef max
#endif

#define _ms					_s / 1000
#define _s					* CLOCKS_PER_SEC
#define _m					_s * 60
#define _h					_m * 60
//#define _d					_h * 24

class Time {
public:
	typedef u32 Val;

private:
	Val _cachedTime;
	Val _delta;
	float _deltaf;

public:


	Time() {
		_deltaf = 0.f;
		_delta = 0;
		seed();
	}

	void seed() {
		auto now = timeGetTime();
		_deltaf = (now - _cachedTime) / 1000.f;
		_delta = now - _cachedTime;
		_cachedTime = now;
	}

	Val now() {
		return _cachedTime;
	}

	Val delta() {
		return _delta;
	}

	float deltaf() {
		return _deltaf;
	}

	Val elapsed(Val t) {
		if(_cachedTime < t)
			return t + (std::numeric_limits<Val>::max() - _cachedTime); // 32-bit looping
		else
			return (_cachedTime - t);
	}

	bool elapsed(Val t, Val period) {
		return elapsed(t) >= period;
	}

	bool elapsed(Val & t, Val period, bool upd) {
		auto res = elapsed(t) >= period;
		if(res) {
			t = now();
		}
		return res;
	}
};