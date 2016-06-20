#include "stdafx.h"
#include "Scene.h"
#include "SMain.h"
#include "GUI.h"
#include "Driver.h"

Scene::Scene() : _wnd(g_wnd.get())
{

}

Scene::~Scene()
{
	deinitialize();
}

void Scene::Handler::init() {
	m_list[ID::Main] = uMake<SMain>();
	//g_game = new class Game;
	//m_list[ID::Game].reset(g_game);
	set(ID::Main);
}

void Scene::Handler::set(Scene * scene) {
	if (_ptr) 
	{
		_ptr->deinitialize();
	}

	g_ui._desk.RemoveAll();
	_ptr = scene;
	_ptr->initialize();
}

void Scene::Handler::set(ID id)
{
	set(get(id));
}


void Scene::Handler::clear() {
	for (auto & p : m_list)
		p.reset();

	_ptr = nullptr;
}

Scene * Scene::Handler::get()
{
	return _ptr;
}

Scene * Scene::Handler::get(ID id)
{
	return m_list[id].get();
}

bool Scene::Handler::is(ID id)
{
	return _ptr == get(id);
}

void Scene::Handler::tick()
{
	_ptr->tick();
}

void Scene::Handler::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	_ptr->display();
}

void Scene::Handler::handle_event(sf::Event & event)
{
	_ptr->handle_event(event);
}

void Scene::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	display();
}

void Scene::toggle_window(sfg::Window::Ptr & wnd, bool flag) {
	wnd->Show(flag);
	/*if (!flag) {
		uievent_false();
	}
	else {
		if (get_window_size(wnd).contains(Mouse::getVfPos()))
		{
			uievent_true();
		}
	}*/
}
