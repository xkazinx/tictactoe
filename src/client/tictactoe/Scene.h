#pragma once
#include <array>
#include "SFGUI/Widgets.hpp"

class Scene : public sf::Drawable
{
public:
	typedef uPtr<Scene> Ptr;

	Scene();
	virtual ~Scene();

	enum ID {
		None = -1,
		Main,
		Game,
		Max
	};

	virtual void initialize() = 0;
	virtual void tick() {}
	virtual void deinitialize() { }
	virtual void display() const = 0;
	virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const override;
	virtual void handle_event(sf::Event & event) {}
	void toggle_window(sfg::Window::Ptr & wnd, bool flag);
	sf::RenderWindow * _wnd;

	class Handler : public sf::Drawable{
	public:
		Handler() : _ptr(nullptr) {}
		std::array<Ptr, ID::Max> m_list;

		Scene * get();
		Scene * get(ID id);

		void set(Scene * scene);
		void set(ID id);

		bool is(ID id);

		void init();
		void tick();
		virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const override;
		void clear();
		void handle_event(sf::Event & event);

	private:
		Scene * _ptr;
	};
};

extern Scene::Handler g_scene;