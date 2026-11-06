# The tictactoe server for Linux (the deployed one: kazin.dev/projects/tictactoe).
# Windows builds the server from tictactoe_server.vcxproj (build.ps1); this file is
# for Linux, built in a container by build_linux.ps1 (Ubuntu 26.04, as the server):
#   cmake -S src/server -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build
cmake_minimum_required(VERSION 3.21)
project(tictactoe_server CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TTT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(KAZILIB ${TTT_SRC}/dep/kazilib)

file(GLOB SERVER_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tictactoe_server/*.cpp)
list(FILTER SERVER_SOURCES EXCLUDE REGEX "stdafx\\.cpp$")

# kazilib: enet (the Unix socket layer), the packet layer, the socket, the
# WebSocket gateway, the log's output
set(KAZILIB_SOURCES
	${KAZILIB}/enet/callbacks.cpp
	${KAZILIB}/enet/compress.cpp
	${KAZILIB}/enet/host.cpp
	${KAZILIB}/enet/list.cpp
	${KAZILIB}/enet/packet.cpp
	${KAZILIB}/enet/peer.cpp
	${KAZILIB}/enet/protocol.cpp
	${KAZILIB}/enet/unix.cpp
	${KAZILIB}/lib/shared/src/net/net_packet.cpp
	${KAZILIB}/lib/shared/src/net/socket.cpp
	${KAZILIB}/lib/shared/src/net/ws_gateway.cpp
	${KAZILIB}/lib/ml/src/log_out.cpp
)

add_executable(tictactoe_server ${SERVER_SOURCES} ${KAZILIB_SOURCES})
target_include_directories(tictactoe_server PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/tictactoe_server
	${TTT_SRC}/shared
	${KAZILIB}/lib/shared/include
	${KAZILIB}/lib/ml/include
	${KAZILIB}/enet/include
	${KAZILIB}/lib/ml/src
)
target_compile_definitions(tictactoe_server PRIVATE HAS_SOCKLEN_T=1)
find_package(Threads REQUIRED)
target_link_libraries(tictactoe_server PRIVATE Threads::Threads)
# One file for the server: the C++ runtime inside (no libstdc++ version needed there)
target_link_options(tictactoe_server PRIVATE -static-libstdc++ -static-libgcc)
