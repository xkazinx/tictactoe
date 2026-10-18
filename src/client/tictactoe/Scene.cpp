#include "stdafx.h"
#include "Scene.h"
#include "SMain.h"
#include "GUI.h"
#include "Driver.h"

Scene::Scene() : _wnd(g_wnd.get())
{
}

void Scene::Deinitialize()
{
	g_ui._desk.RemoveAll();
}

void Scene::toggle_window(sfg::Window::Ptr & wnd, const bool flag)
{
	wnd->Show(flag);
}

Scenes::Scenes() = default;
Scenes::~Scenes() = default;

void Scenes::Init()
{
	_main = make_unique<SMain>();
	s_main = _main.get();
	_mgr.Add(*_main);
	_mgr.Set(*_main);
}

void Scenes::Clear()
{
	s_main = nullptr;
	_main.reset();
}
