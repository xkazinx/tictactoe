// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here

#include <tdef.h>
#include "SFGUI/SFGUI.hpp"
#include "SFML/Graphics.hpp"
#include <ctime.h>
extern Time Clock;
#define res_x 800
#define res_y 600
#include "enet/enet.h"
#include "packet.h"
#include "enet/Socket.h"
#include <iostream>
#define cout std::cout