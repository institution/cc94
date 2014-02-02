#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <vector>
#include <map>
#include <boost/cstdint.hpp>
#include <boost/multi_array.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "col.h"
#include "iomm.h"
#include "text.h"
#include "env.h"

using std::unique_ptr;

using std::ifstream;
using std::ofstream;
using std::string;
using std::add_pointer;
using std::to_string;

using std::cout;
using std::endl;

using boost::format;

using std::pair;
using std::map;
using std::vector;



//------------------------------------------------------------------------------

namespace col{
	using namespace iomm;


	using Res = map<pair<string,uint>,sf::Image>;
	Res g_res;

	const string 
		TERR = "COLONIZE/TERRAIN_SS",
		ICON = "COLONIZE/ICONS_SS";

	const Res::mapped_type& res(const string &d, const uint &i) {

		auto key = Res::key_type(d,i);
		auto p = g_res.find(key);	
		if (p != g_res.end()) {
			return (*p).second;
		}
		else {
			auto &img = g_res[key];
			auto path = str(format("./%||/%|03|.png") % d % i);
			if (!img.LoadFromFile(path)) {
				throw std::runtime_error("cant't load image file: " + path);
			}
			return img;		
		}
	}



	struct Game{
		Env env;
		
		Game() {
			
		}
		
		
		
		
	};

	//void new_icon(Game &game, const Icon &icon) {
	//	auto &icons = game.env.icons;
	//	icons.push_back(icon);  //copy
	//	icons[icons.size()-1].id = icons.size()-1;  // id
	//}
	
	void move(Game &game, uint32 icon_id, uint8 dx, uint8 dy) {
		auto &icon = game.env.icons[icon_id];
		icon.x += dx;
		icon.y += dy;
	}

	void end_turn() {

	}

		
	
	/*
	void exec(Env &env, Action &a, long &auth_key)

	


	void turn(Env &env) {
		for (auto &p: env.players) {

			auto a = player_if.retrive();
			auto r = exec_action(a);
			player_if.send(r);

		}
		env.turn += 1;

	}

	*/

	
	
	


	using Terrain = boost::multi_array<uint8, 2>;

	/*
	struct AuthInfo{
		using SharedKey = long;
		using PlayerId = uint32;

		map<SharedKey,PlayerId> info;

		bool authenticate(PlayerId player_id, SharedKey token) {
			auto it = info.find(token)
			if (it != info.end()) {
				return (*it).second == player_id;
			}
			return false;
		}
	};
	*/












	void render_icon(sf::RenderWindow &win, const Icon &icon) {
		sf::Sprite s;
		s.SetPosition(icon.x*16, icon.y*16);
		s.SetImage(res(ICON, icon.type));
		win.Draw(s);
	}

	void render_map(sf::RenderWindow &win, const Env &env) 
	{
		auto &terrain = env.terr;

		auto w = env.w;
		auto h = env.h;

		for (uint i = 0; i < w; ++i) {
			for (uint j = 0; j < h; ++j) {
				sf::Sprite s;
				s.SetPosition(i*16, j*16);
				s.SetImage(res(TERR, terrain[i][j]));
				win.Draw(s);
			}
		}

		for (auto &p: env.icons) {
			render_icon(win, p.second);
		}

	}


	void render_cursor(sf::RenderWindow &win, const Env &env) {

	}
	
	
	using boost::split;
	using boost::is_any_of;
	
	struct Console{
		string buffer;
		Env &env;

		Console(Env &env_): buffer(""), env(env_) {

		}
		
		void handle(uint16 code) {
			if (code == 13) {
				cout << endl;
				
				vector<string> es;
				split(es, buffer, is_any_of(" "));
					
				if (es[0] == "list") {
					for (auto &icon: env.icons) {
						cout << icon.second << endl;
						
					}
				}
				else
				if (es[0] == "del") {
					env.icons.erase(IconKey(std::stoi(es[1])));
				}
				
				buffer = "";	
				cout << "> ";				
			}			
			else {
				cout << char(code);
				buffer.push_back(char(code));
			}
			
			cout.flush();
		}
		
	};
	

	void handle_event(sf::RenderWindow &app, Env &env, const sf::Event &ev, Console &con) {
		if (ev.Type == sf::Event::TextEntered) {
			con.handle(ev.Text.Unicode);			
		}		

		if (ev.Type == sf::Event::KeyPressed) {
			if (ev.Key.Code == sf::Key::Escape) {
				app.Close();
			}				
		}
		else
		if (ev.Type == sf::Event::KeyReleased) {
			if (ev.Key.Code == sf::Key::C) {
				env.add(
					Icon(env.icons.size(), 100, 5, 5)
				);
			}				
		}
		else
		if (ev.Type == sf::Event::Closed) {
			app.Close();
		}
	}

}


int main()
{
	using namespace col;
	
	string fname("./aaa.mp");
	
	 
	sf::Font tiny;
	if (!tiny.LoadFromFile("font/pixelated/pixelated.ttf", 8)) {
		throw std::runtime_error("cannot load font");
	}
	
	Text t;
	t.text = string("COLONIST: BUILD   MOVE   PAUSE");
	t.pos = sf::Vector2i(0,190);
	t.f = &tiny;
	
	
	
	
	const uint SCL = 3;
	sf::RenderWindow app(sf::VideoMode(320 * SCL, 200 * SCL, 32), "SFML Window");
	sf::View view(sf::FloatRect(0, 0, 320, 200));
	app.SetView(view);
	
	ifstream f(fname, std::ios::binary);
	auto env = read_env(f);
	f.close();
    
	Console con(env);
	
	app.Clear();
	render_map(app, env);
	render_text(app, t);
	app.Display();
	
	while (app.IsOpened())
	{
		sf::Event ev;
		while (app.GetEvent(ev)) {
			handle_event(app, env, ev, con);
		}		
		
	}

	{
		ofstream fo(fname, std::ios::binary);
		write_env(fo, env);
	}
		
	return EXIT_SUCCESS;
}

