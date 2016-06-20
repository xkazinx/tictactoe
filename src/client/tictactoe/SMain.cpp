#include "stdafx.h"
#include "SMain.h"
#include "SFGUI/Widgets.hpp"
#include "GUI.h"
#include "Net.h"

#define btn_click(btn, btnid) btn->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&SMain::click, this, ButtonID::btnid));

SMain::SMain() : _entry_name(sfg::Entry::Create())
{

}


SMain::~SMain()
{
}

void SMain::initialize() {
	using namespace sfg;

	{
		auto & wnd = getWnd(Windows::LogIn);
		wnd = sfg::Window::Create(sfg::Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR);
		wnd->SetTitle("Authentication");
		wnd->SetRequisition(sf::Vector2f(res_x / 3, res_y / 6));
		wnd->SetPosition(sf::Vector2f((res_x / 2) - ((res_x / 2) / 3), (res_y / 2) - ((res_y / 2) / 6)));
		auto wrapper = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);

		auto auth_table = Table::Create();
		auth_table->SetRowSpacings(5.f);

		auto game_btn = Button::Create(L"Log In");
		auto label = Label::Create(L"Nickname:");

		auth_table->Attach(label, sf::Rect<sf::Uint32>(0, 0, 1, 1));
		auth_table->Attach(_entry_name, sf::Rect<sf::Uint32>(1, 0, 3, 1), sfg::Table::FILL);
		auth_table->Attach(game_btn, sf::Rect<sf::Uint32>(0, 1, 3, 1), 3, sfg::Table::EXPAND | sfg::Table::EXPAND);

		btn_click(game_btn, LogIn);

		wrapper->Pack(auth_table);
		wnd->Add(wrapper);
		g_ui._desk.Add(wnd);
	}

	{
		auto & wnd = getWnd(Windows::Connecting);
		wnd = sfg::Window::Create(Window::Style::BACKGROUND);
		wnd->SetTitle("Connecting to Server");
		wnd->SetRequisition(sf::Vector2f(400, 150));
		wnd->SetPosition(sf::Vector2f((res_x / 2) - (400 / 2), (res_y / 2) - 150 / 2));
		g_ui._desk.Add(wnd);
		wnd->Show(false);

		auto box = Box::Create(Box::Orientation::HORIZONTAL);
		auto sp = Spinner::Create();
		sp->Start();
		
		auto align = Alignment::Create();
		align->SetScale(sf::Vector2f(0.5f, 0.5f));
		align->SetAlignment(sf::Vector2f(5.f, .5f));
		align->Add(sp);

		box->Pack(align);
		box->Pack(Label::Create(L"Connecting to Server, please wait..."));
		wnd->Add(box);
	}
}

void SMain::display() const {

}

void SMain::click(ButtonID id)
{
	switch (id)
	{
	case ButtonID::LogIn:
		string name = _entry_name->GetText();
		if (name.size() == 0)
			return;

		toggle_window(getWnd(Windows::LogIn), false);
		toggle_window(getWnd(Windows::Connecting), true);
		g_net._sock.connect();
		break;
	}
}