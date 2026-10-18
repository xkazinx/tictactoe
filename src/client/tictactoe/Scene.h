#pragma once
#include <scene/manager.h>
#include "SFGUI/Widgets.hpp"

// A scene on kazilib's scene manager (lib/shared/include/scene/manager.h):
// Initialize on entry, Render each frame, HandleEvent with an sf::Event
class Scene : public lib::scene::Base
{
public:
	Scene();

	// Leaving it: its windows leave the desktop
	void Deinitialize() override;
	void toggle_window(sfg::Window::Ptr & wnd, const bool flag);

	sf::RenderWindow * _wnd;
};

class SMain;

// The client's scenes and the manager that switches them
struct Scenes
{
	lib::scene::Manager _mgr;
	unique_ptr<SMain> _main;

	// In Scene.cpp, where SMain is complete (unique_ptr checks the delete there)
	Scenes();
	~Scenes();

	void Init();
	void Clear();
	void Render() { _mgr.Render(); }
	void HandleEvent(const sf::Event & event) { _mgr.HandleEvent(&event); }
};

extern Scenes g_scene;
