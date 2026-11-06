# The tictactoe client for a web browser (WebAssembly, Emscripten). Windows builds
# the client from tictactoe.vcxproj (build.ps1); this file is the web one, run by
# build_web.ps1 against kazilib's SFML / SFGUI web builds (kazilib web/build_deps.ps1):
#   emcmake cmake -S src/client -B <build> -G Ninja -DSFML_DIR=... -DSFGUI_DIR=...
# Output: index.html / .js / .wasm / .data (the font) - the folder the server's
# gateway serves (server\web\) and kazin.dev/projects/tictactoe.
cmake_minimum_required(VERSION 3.21)
project(tictactoe_web CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TTT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(SFML_STATIC_LIBRARIES ON)
find_package(SFML 3 REQUIRED COMPONENTS Graphics)
find_package(SFGUI REQUIRED)
include(${TTT_SRC}/dep/kazilib/web/kazilib_web.cmake)

file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tictactoe/*.cpp)
list(FILTER CLIENT_SOURCES EXCLUDE REGEX "stdafx\\.cpp$")

add_executable(tictactoe ${CLIENT_SOURCES})
kazilib_web_target(tictactoe)
target_include_directories(tictactoe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tictactoe ${TTT_SRC}/shared)
target_link_libraries(tictactoe PRIVATE SFGUI::SFGUI SFML::Graphics)
target_link_options(tictactoe PRIVATE
	"--preload-file=${TTT_SRC}/../deploy/fonts/comic.ttf@/fonts/comic.ttf"
	"--preload-file=${TTT_SRC}/../deploy/sprites/background.jpg@/sprites/background.jpg")
