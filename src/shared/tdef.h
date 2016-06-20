#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
typedef unsigned long long u64;
typedef signed long long i64;

typedef unsigned long u32;
typedef signed long i32;

typedef unsigned short u16;
typedef signed short i16;

typedef unsigned char u8;
typedef signed char i8;

#include <vector>
#define Vector std::vector
#include <string>
typedef std::string string;
#include <memory>
#define uPtr std::unique_ptr
#define uMake std::make_unique
#include <sstream>
#include <algorithm>
typedef std::stringstream sstream;
#include <fstream>
typedef std::ifstream ifstream;
typedef std::ofstream ofstream;
#define _WINSOCK2API_
#include <windows.h>

#define _IP "127.0.0.1"
#define _PORT 6987

enum class Msg
{
	Auth,
};