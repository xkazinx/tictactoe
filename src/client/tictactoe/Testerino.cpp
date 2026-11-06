#include "stdafx.h"
#include "Testerino.h"
#include "App.h"
#include "Driver.h"
#include "GUI.h"
#include "Net.h"
#include "Player.h"
#include "SMain.h"
#ifndef _WIN32
// The web build: no bridge (a browser has no endpoint folder); kazilib's
// web/tools/browser.js drives the page instead
namespace testerino
{
	void Start() {}
	void Pump() {}
	void AfterDraw(sf::RenderWindow &) {}
	void Stop() {}
	bool Active() { return false; }
}
#else
#include <testerino/tcore.h>

namespace testerino
{
	namespace
	{
		using tcore::JArr;
		using tcore::JObj;

		tcore::Endpoint g_ep;
		tcore::Runner g_run(g_ep);
		bool g_active = false;
		string g_windows_seen;

		struct Shot
		{
			string _path;
			tcore::Job * _job = nullptr;
		};
		vector<Shot> g_shots;

		// ---- What is on screen ---------------------------------------------------

		string WindowName(const SMain::Windows w)
		{
			return string(enum_name(w));
		}

		string ShownWindows()
		{
			string s;

			for (const auto w : enum_values<SMain::Windows>())
			{
				if (w != SMain::Windows::None && w != SMain::Windows::Max && s_main && s_main->is_shown(w))
					s += (s.empty() ? "" : ",") + WindowName(w);
			}

			return s;
		}

		string Board()
		{
			string s;

			for (int x = 0; x < 3; x++)
			{
				if (x)
					s += '/';

				for (int y = 0; y < 3; y++)
				{
					const string l = ToUtf8(s_main->_btn[x][y]->GetLabel());
					s += l == "X" || l == "O" ? l : ".";
				}
			}

			return s;
		}

		struct WidgetInfo
		{
			sfg::Widget::Ptr _w;
			string _window, _type, _id, _text;
			sf::FloatRect _rect;
			bool _enabled = true;
		};

		// `offset` and `clip`: inside a Viewport (the lobby's scrolled list) SFGUI's
		// positions are the viewport's own, so its screen spot minus its scroll is
		// added, and a widget scrolled out of its view counts as not visible
		void Collect(const sfg::Widget::Ptr & w, const string & window, vector<WidgetInfo> & out, const sf::Vector2f offset = {}, const optional<sf::FloatRect> clip = {})
		{
			if (!w || !w->IsGloballyVisible())
				return;

			WidgetInfo i;
			i._w = w;
			i._window = window;
			i._type = w->GetName();
			i._id = w->GetId();
			i._rect = sf::FloatRect(w->GetAbsolutePosition() + offset, w->GetAllocation().size);

			if (clip)
			{
				const sf::Vector2f mid = i._rect.position + i._rect.size / 2.f;
				if (!clip->contains(mid))
					return;
			}

			// A ScrolledWindow lists its content without the Viewport between them, and
			// the Viewport answers 0,0 as its own absolute position (its children's space):
			// its screen spot is the ScrolledWindow's plus its place inside it
			if (auto sw = std::dynamic_pointer_cast<sfg::ScrolledWindow>(w))
			{
				const auto v = sw->GetViewport();
				out += i;
				const sf::Vector2f origin = i._rect.position + v->GetAllocation().position;
				const sf::Vector2f scroll(v->GetHorizontalAdjustment()->GetValue(), v->GetVerticalAdjustment()->GetValue());
				const sf::FloatRect view(origin, v->GetAllocation().size);

				for (const auto & child : v->GetChildren())
					Collect(child, window, out, origin - scroll, view);

				return;
			}
			i._enabled = w->GetState() != sfg::Widget::State::INSENSITIVE;

			if (auto b = std::dynamic_pointer_cast<sfg::Button>(w))
				i._text = ToUtf8(b->GetLabel());
			else if (auto e = std::dynamic_pointer_cast<sfg::Entry>(w))
				i._text = ToUtf8(e->GetText());
			else if (auto l = std::dynamic_pointer_cast<sfg::Label>(w))
				i._text = ToUtf8(l->GetText());
			else if (auto f = std::dynamic_pointer_cast<sfg::Frame>(w))
				i._text = ToUtf8(f->GetLabel());
			else if (auto win = std::dynamic_pointer_cast<sfg::Window>(w))
				i._text = ToUtf8(win->GetTitle());

			if (i._type != "Box" && i._type != "Table" && i._type != "Viewport" && i._type != "Alignment")
				out += i;

			if (auto c = std::dynamic_pointer_cast<sfg::Container>(w))
			{
				for (const auto & child : c->GetChildren())
					Collect(child, window, out, offset, clip);
			}
		}

		vector<WidgetInfo> Widgets()
		{
			vector<WidgetInfo> out;

			for (const auto w : enum_values<SMain::Windows>())
			{
				if (w != SMain::Windows::None && w != SMain::Windows::Max && s_main && s_main->is_shown(w))
					Collect(s_main->getWnd(w), WindowName(w), out);
			}

			return out;
		}

		JObj WidgetJson(const WidgetInfo & i)
		{
			JObj o;
			o.Str("window", i._window).Str("type", i._type).Str("id", i._id).Str("text", i._text);
			o.Int("x", (long long)i._rect.position.x).Int("y", (long long)i._rect.position.y);
			o.Int("w", (long long)i._rect.size.x).Int("h", (long long)i._rect.size.y).Bool("enabled", i._enabled);
			return o;
		}

		// A widget by id, else a button by its text (case-insensitive), the topmost
		// window's first: the Msg / Winner windows are drawn over the others
		const WidgetInfo * FindWidget(const vector<WidgetInfo> & all, const string & what)
		{
			const string low = tcore::Lower(what);

			for (auto it = all.rbegin(); it != all.rend(); ++it)
			{
				if (it->_id == what)
					return &*it;
			}

			for (auto it = all.rbegin(); it != all.rend(); ++it)
			{
				if (it->_type == "Button" && tcore::Lower(it->_text) == low)
					return &*it;
			}

			return nullptr;
		}

		JObj State()
		{
			JObj o;
			o.Str("windows", ShownWindows()).Bool("connected", g_net._sock && g_net._sock->IsConnected());
			o.Str("name", g_player._name).Str("opponent", g_player._to_name).Str("entry", ToUtf8(s_main->_entry_name->GetText()));
			o.Str("mark", s_main->_identifier).Bool("turn", s_main->_is_turn).Bool("finished", s_main->_game_finished);
			o.Str("board", Board()).Str("vs", ToUtf8(s_main->_vs_label->GetText())).Str("turn_label", ToUtf8(s_main->_turn_label->GetText()));
			o.Str("end", ToUtf8(s_main->_end_label->GetText())).Bool("again_enabled", s_main->_btn_again->GetState() != sfg::Widget::State::INSENSITIVE);
			o.Str("msg_title", ToUtf8(s_main->_msg_title->GetLabel())).Str("msg", ToUtf8(s_main->_msg_content->GetText()));
			o.Str("msg_back", WindowName(s_main->_msg_back)).Str("lobby_title", ToUtf8(s_main->getWnd(SMain::Windows::Select)->GetTitle()));
			JArr players;

			for (const auto & p : s_main->_players)
				players.Obj(JObj().Str("name", p._name).Bool("in_game", p._in_game));

			o.Raw("players", players.Done());
			return o;
		}

		// ---- Input, through App::process_event as real input goes -----------------

		void Mouse(const sf::Vector2i at, const int what)
		{
			if (what == 0)
				g_app.process_event(sf::Event::MouseMoved{ at });
			else if (what == 1)
				g_app.process_event(sf::Event::MouseButtonPressed{ sf::Mouse::Button::Left, at });
			else
				g_app.process_event(sf::Event::MouseButtonReleased{ sf::Mouse::Button::Left, at });
		}

		// Move, press and release on three frames, as a hand does
		void Click(tcore::Job & j, const sf::Vector2i at, JObj answer)
		{
			j.Add([at] { Mouse(at, 0); return true; });
			j.Add([at] { Mouse(at, 1); return true; });
			j.Add([at] { Mouse(at, 2); return true; });
			j.Add([&j, answer] { j.Ok(answer); return true; });
		}

		bool KeyCode(const string & name, sf::Keyboard::Key & code)
		{
			const string n = tcore::Lower(name);
			static const std::map<string, sf::Keyboard::Key> kKeys = {
				{ "enter", sf::Keyboard::Key::Enter }, { "escape", sf::Keyboard::Key::Escape }, { "backspace", sf::Keyboard::Key::Backspace },
				{ "tab", sf::Keyboard::Key::Tab }, { "left", sf::Keyboard::Key::Left }, { "right", sf::Keyboard::Key::Right },
				{ "home", sf::Keyboard::Key::Home }, { "end", sf::Keyboard::Key::End }, { "delete", sf::Keyboard::Key::Delete },
			};
			const auto it = kKeys.find(n);
			if (it == kKeys.end())
				return false;
			code = it->second;
			return true;
		}

		// Code and scancode both, as a real key event has them (SFGUI's Entry reads
		// the scancode: Backspace, Delete, the arrows)
		void Key(const sf::Keyboard::Key code, const bool down)
		{
			const sf::Keyboard::Scancode scan = sf::Keyboard::delocalize(code);
			if (down)
			{
				sf::Event::KeyPressed k{};
				k.code = code;
				k.scancode = scan;
				g_app.process_event(k);
			}
			else
			{
				sf::Event::KeyReleased k{};
				k.code = code;
				k.scancode = scan;
				g_app.process_event(k);
			}
		}

		// ---- Verbs and conditions ----------------------------------------------------

		void AddVerbs()
		{
			g_run.Add("state", "state", "the game's state: windows shown, name, opponent, board (rows X / O / .), labels, message, lobby", [](const tcore::Cmd &, tcore::Job & j) {
				j.Ok(State());
			});

			g_run.Add("widgets", "widgets [window]", "the visible widgets of the shown windows: window, type, id, text, x y w h (screen), enabled", [](const tcore::Cmd & c, tcore::Job & j) {
				JArr a;

				for (const auto & i : Widgets())
				{
					if (c.Arg(0).empty() || tcore::Lower(i._window) == tcore::Lower(c.Arg(0)))
						a.Obj(WidgetJson(i));
				}

				j.Ok(JObj().Raw("widgets", a.Done()));
			});

			g_run.Add("click", "click <x> <y>", "a left click at that window spot (move, press, release on three frames)", [](const tcore::Cmd & c, tcore::Job & j) {
				int x = 0, y = 0;
				if (!c.ArgInt(0, x) || !c.ArgInt(1, y))
					return j.Fail("click <x> <y>");
				Click(j, { x, y }, JObj().Int("x", x).Int("y", y));
			});

			g_run.Add("press", "press <id | button text>", "clicks the middle of a visible widget (an id from widgets: name, cell12, player_bob..., or a button's text: \"Log In\", Ok, \"Play Again\", \"Go to Lobby\")", [](const tcore::Cmd & c, tcore::Job & j) {
				// The words joined: press Log In and press "Log In" are the same
				string what;

				for (const auto & a : c.args)
					what += (what.empty() ? "" : " ") + a;

				const auto all = Widgets();
				const auto w = FindWidget(all, what);
				if (!w)
					return j.Fail("no visible widget '" + what + "'", JObj().Str("windows", ShownWindows()));
				const sf::Vector2i at((int)(w->_rect.position.x + w->_rect.size.x / 2), (int)(w->_rect.position.y + w->_rect.size.y / 2));
				Click(j, at, WidgetJson(*w));
			});

			g_run.Add("type", "type <text>", "types the text into the focused entry (one TextEntered per character)", [](const tcore::Cmd & c, tcore::Job & j) {
				for (const char32_t cp : lib::utf8::ToUtf32(c.rest))
					g_app.process_event(sf::Event::TextEntered{ cp });
				j.Frame();
				j.Add([&j] { j.Ok(JObj().Str("entry", ToUtf8(s_main->_entry_name->GetText()))); return true; });
			});

			g_run.Add("key", "key <enter|escape|backspace|tab|left|right|home|end|delete> [times]", "a key pressed and let go", [](const tcore::Cmd & c, tcore::Job & j) {
				sf::Keyboard::Key code{};
				if (!KeyCode(c.Arg(0), code))
					return j.Fail("unknown key " + c.Arg(0));
				int times = 1;
				c.ArgInt(1, times);

				for (int i = 0; i < std::max(1, times); i++)
				{
					j.Add([code] { Key(code, true); return true; });
					j.Add([code] { Key(code, false); return true; });
				}
			});

			g_run.Add("shot", "shot <name>", "a screenshot of the next frame (shots\\<name>.png)", [](const tcore::Cmd & c, tcore::Job & j) {
				if (c.Arg(0).empty())
					return j.Fail("shot <name>");
				g_shots += Shot{ g_ep.ShotPath(c.Arg(0)).string(), &j };
				j.Add([&j] { return j.answered; }); // AfterDraw answers it
				j.OnAbort([&j] { std::erase_if(g_shots, [&j](const Shot & s) { return s._job == &j; }); });
			});

			g_run.Add("close", "close", "closes the window, as its X button does (the program ends)", [](const tcore::Cmd &, tcore::Job & j) {
				j.Ok();
				g_app.process_event(sf::Event::Closed{});
			});

			g_run.Add("quit", "quit", "ends the program", [](const tcore::Cmd &, tcore::Job & j) {
				j.Ok();
				g_wnd.get()->close();
			});

			g_run.AddCond("window", "window=<LogIn|Connecting|Select|Msg|Game|Winner>: shown", [](const string & v, char, tcore::Cond & out, string &) {
				out = [v](string & d) {
					d = ShownWindows();
					return tcore::Lower("," + d + ",").find(tcore::Lower("," + v + ",")) != string::npos;
				};
				return true;
			});

			auto text_cond = [](const char * key, std::function<string()> get) {
				g_run.AddCond(key, string(key) + "=<text> / " + key + "~<part>", [get](const string & v, const char op, tcore::Cond & out, string &) {
					out = [v, op, get](string & d) {
						d = get();
						return op == '~' ? tcore::Lower(d).find(tcore::Lower(v)) != string::npos : d == v;
					};
					return true;
				});
			};
			text_cond("msg", [] { return ToUtf8(s_main->_msg_content->GetText()); });
			text_cond("end", [] { return ToUtf8(s_main->_end_label->GetText()); });
			text_cond("board", [] { return Board(); });
			text_cond("name", [] { return g_player._name; });
			text_cond("turn", [] { return string(s_main->_is_turn ? "1" : "0"); });
			text_cond("players", [] {
				string s;

				for (const auto & p : s_main->_players)
					s += (s.empty() ? "" : ",") + p._name + (p._in_game ? "*" : "");

				return s;
			});

			g_run.ping_fields = [](JObj & o) { o.Str("windows", ShownWindows()); };
		}
	}

	void Start()
	{
		const int slot = (int)lib::init::Number("testerino", 0);
		if (slot <= 0)
			return;

		std::error_code ec;
		if (!tcore::fs::exists(tcore::CheckoutFromExe() / "src" / "tictactoe.sln", ec))
		{
			cout << "(!) testerino is only on in a source checkout (src\\tictactoe.sln)\n";
			return;
		}

		AddVerbs();
		if (!g_ep.Open(tcore::CheckoutFromExe() / "testerino", "client_" + to_string(slot), JObj().Int("slot", slot)))
		{
			cout << "(!) testerino: cannot open its folder\n";
			return;
		}

		g_active = true;
		cout << "Testerino: listening on " << g_ep._dir.string() << "\n";
	}

	void Pump()
	{
		if (!g_active)
			return;

		g_run.Pump();

		// An event each time the shown windows change, for `events` and routines
		const string shown = ShownWindows();
		if (shown != g_windows_seen)
		{
			g_windows_seen = shown;
			g_ep.Event("windows", JObj().Str("shown", shown));
		}
	}

	void AfterDraw(sf::RenderWindow & wnd)
	{
		if (g_shots.empty())
			return;

		static sf::Texture texture;
		const auto sz = wnd.getSize();
		if (texture.getSize() != sz && !texture.resize(sz))
			return;
		texture.update(wnd);
		const sf::Image image = texture.copyToImage();

		tcore::Rgba img;
		img.w = (int)image.getSize().x;
		img.h = (int)image.getSize().y;
		img.px.assign(image.getPixelsPtr(), image.getPixelsPtr() + (size_t)img.w * img.h * 4);

		for (auto & s : g_shots)
		{
			if (tcore::WritePng(s._path, img))
				s._job->Ok(JObj().Str("path", s._path).Int("w", img.w).Int("h", img.h));
			else
				s._job->Fail("could not write " + s._path);
		}

		g_shots.clear();
	}

	void Stop()
	{
		if (g_active)
			g_ep.Close();
		g_active = false;
	}

	bool Active()
	{
		return g_active;
	}
}
#endif
