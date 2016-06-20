#include "stdafx.h"
#include "SMain.h"
#include "SFGUI/Widgets.hpp"
#include "GUI.h"

SMain::SMain()
{
}


SMain::~SMain()
{
}


void SMain::initialize() {
	using namespace sfg;
	sfg::Window::Ptr wnd = sfg::Window::Create(sfg::Window::Style::NO_STYLE);
	wnd->SetRequisition(sf::Vector2f(res_x / 2, res_y / 1.3f));
	wnd->SetPosition(sf::Vector2f((res_x / 2) - ((res_x / 2) / 2), (res_y / 2) - ((res_y / 1.3f) / 2)));
	auto wrapper = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);

	auto auth_table = Table::Create();
	auth_table->SetRowSpacings(5.f);

	auto game_btn = Button::Create(L"Play Game");
	//game_btn->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&SMain::btn_Game, this));
	auto map_btn = Button::Create(L"Map Editor");
	//map_btn->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&SMain::btn_MapEditor, this));

	auth_table->Attach(game_btn, sf::Rect<sf::Uint32>(0, 1, 1, 3), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL);
	auth_table->Attach(map_btn, sf::Rect<sf::Uint32>(0, 4, 1, 3), sfg::Table::EXPAND | sfg::Table::FILL, sfg::Table::FILL);

	wrapper->Pack(auth_table);
	wnd->Add(wrapper);
	g_ui._desk.Add(wnd);
}

void SMain::display() const {

}