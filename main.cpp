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
#include "csv.h"

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
			img.SetSmooth(false);
			return img;		
		}
	}



	void render_shield(sf::RenderWindow &win, uint16 x, uint16 y, const Color &col) {
		win.Draw(
			sf::Shape::Rectangle(
				x+1, y+1, 
				x+6, y+8, 
				sf::Color(col.r,col.g,col.b, 255),
				1, 
				sf::Color(0,0,0, 255)
			)
		);
	}

	void render_icon(sf::RenderWindow &win, const Env &env, const Icon &icon) {
		auto &p = env.get_player(icon.player_id);
		render_shield(win, icon.x*16, icon.y*16, p.color);
		
		sf::Sprite s;
		s.SetPosition(icon.x*16, icon.y*16);
		s.SetImage(res(ICON, icon.type_id));
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
			render_icon(win, env, p.second);
		}

	}


	void render_cursor(sf::RenderWindow &win, const Env &env) {

	}
	
	
	Color make_color_by_name(const string &s) {
		if (s == "red") {
			return Color(255,0,0);
		}
		else
		if (s == "green") {
			return Color(0,255,0);
		}
		else
		if (s == "blue") {
			return Color(0,0,255);
		}
		else {
			return Color(0,0,0);
		}
	}
	
	
	using boost::split;
	using boost::is_any_of;
	using std::stoi;
	
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
					
				if (es[0] == "lsi") {
					for (auto &icon: env.icons) {
						cout << icon.second << endl;						
					}
				}
				else
				if (es[0] == "deli") {
					env.del(std::stoi(es[1]));
				}
				else
				if (es[0] == "addi") {					
					env.add(Icon(
						env.next_key(), 103, 
						std::stoi(es[1]), std::stoi(es[2]), 
						std::stoi(es[3])
					));
				}
				else
				if (es[0] == "lsp") {
					for (auto &p: env.players) {
						cout << p.second << endl;						
					}
				}
				else
				if (es[0] == "delp") {
					env.del_player(std::stoi(es[1]));
				}
				else
				if (es[0] == "addp") {					
					env.add_player(Player(
						env.next_key_player(),
						string(es[1]),
						make_color_by_name(string(es[2]))
					));
				}
				else
				if (es[0] == "set_owner") {
					env.set_owner(
						stoi(es[1]),
						stoi(es[2])
					);
				}
				else
				if (es[0] == "move") {
					
					env.move(					
						stoi(es[1]), // id
						stoi(es[2]), // dx
						stoi(es[3]) // dy
					);
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
				
			}				
		}
		else
		if (ev.Type == sf::Event::Closed) {
			app.Close();
		}
	}

}


void render(sf::RenderWindow &app, const col::Env &env, const Text &t) {
	app.Clear();
	render_map(app, env);
	render_text(app, t);
	app.Display();
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
	auto env = io::read<Env>(f);
	f.close();
    
	Console con(env);
	
	auto last_mod = env.mod - 1;
	
	while (app.IsOpened())
	{
		
		if (env.mod != last_mod) {
			render(app, env, t);
			last_mod = env.mod;
		}
		
		sf::Event ev;
		while (app.GetEvent(ev)) {
			handle_event(app, env, ev, con);
		}		
		
	}

	{
		ofstream fo(fname, std::ios::binary);
		io::write<Env>(fo, env);
	}
		
	return EXIT_SUCCESS;
}

