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
#include "ai.h"

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
		ICON = "COLONIZE/ICONS_SS",
		PHYS = "COLONIZE/PHYS0_SS",
		ARCH = "COLONIZE/PARCH_SS",
		BUILDING = "COLONIZE/BUILDING_SS";

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
				// try default
				if (!img.LoadFromFile("./" + ICON + "/065.png")) {
					throw std::runtime_error("can't load default image file while loading: " + path);
				}
				// throw std::runtime_error("cant't load image file: " + path);
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
	
	
	void render_playerind(sf::RenderWindow &win, uint16 x, uint16 y, const Color &col) {
		win.Draw(
			sf::Shape::Rectangle(
				x, y, 
				x+1, y+1, 
				sf::Color(col.r,col.g,col.b, 255),
				0, 
				sf::Color(0,0,0, 0)
			)
		);
	}
	
	void render_cursor(sf::RenderWindow &win, uint16 x, uint16 y) {
		win.Draw(
			sf::Shape::Rectangle(
				x+1, y+1, 
				x+15, y+15, 
				sf::Color(0,0,0, 0),
				1, 
				sf::Color(255,255,255, 255)
			)
		);
	}
	
	void render_sprite(sf::RenderWindow &win, uint16 x, uint16 y, const sf::Image &img) {
		sf::Sprite s;
		s.SetPosition(x, y);
		s.SetImage(img); 
		win.Draw(s);
	}

	
	
	void render_icon(sf::RenderWindow &win, const Env &env, const Icon &icon) {
		auto &p = env.get_player(icon.player->id);
		
		auto type = icon.type->id;
		switch (type / 256) {
			case 0: 
				render_shield(win, icon.x*16, icon.y*16, p.color);
				render_sprite(win, icon.x*16, icon.y*16, res(ICON, type));
				break;
			case 1: 
				render_sprite(win, icon.x*16, icon.y*16, res(ICON, 4));
				// flag:
				render_sprite(win, icon.x*16+5, icon.y*16, res(ICON, icon.player->flag_id));
				break;
		}
		
	}
	


	struct Console;
	



	
	
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
	
	
	
	
	enum PlayerType{
		HUMAN,
		AI
	};
	
	PlayerType get_player_type(const Player &p) {
		if (p.id == 0) {
			return HUMAN;
		}
		else {
			return AI;
		}
	}
	
		
	
	using boost::split;
	using boost::is_any_of;
	using std::stoi;
	
	struct Console{
		string buffer;
		Env &env;
		Coord sel_x, sel_y;
		uint32 mod;
		Coord city_x, city_y;
		
		Console(Env &env_): buffer(""), env(env_) {
			sel_x = sel_y = -1;
			city_x = city_y = -1;
			mod = 0;
		}
		
		void handle(uint16 code) {
			if (code == 13) {
				cout << endl;
				
				vector<string> es;
				split(es, buffer, is_any_of(" "));
				
				if (es[0] == "help") {
					cout << "icons: lsi, addi, deli id" << endl;
					cout << "players: lsp, addp, delp" << endl;
					cout << "lsut" << endl;
					cout << "move, set_owner" << endl;
				}
				else				
				if (es[0] == "turn") {
					env.exec(action::Turn());
					cout << "Turn: " << env.turn_no << ", " << env.curr_player->name << endl;					
				}
				else
				if (es[0] == "lsi") {
					for (auto &icon: env.icons) {
						cout << icon.second << endl;						
					}
				}
				else
				if (es[0] == "lsut") {
					for (auto &utp: env.uts) {
						auto &ut = utp.second;
						cout << format("%u  %s\n") % uint16(ut.id) % ut.name;
					}
				}
				else
				if (es[0] == "deli") {
					env.destroy_icon(std::stoi(es[1]));
				}
				else
				if (es[0] == "addi") {	
					switch (es.size()) {
						default: {
							cout << "Usage1: addi type_id player_id" << endl;
							cout << "Usage2: addi type_id player_id x y" << endl;						
							break;
						}
						case 3: {
							env.create_icon(
								std::stoi(es.at(1)), // type_id
								std::stoi(es.at(2)),  // player_id
								this->sel_x, this->sel_y // x y
							);
							break;
						}							
						case 5: {
							env.create_icon(
								std::stoi(es.at(1)), // type_id
								std::stoi(es.at(2)),  // player_id
								std::stoi(es.at(3)), std::stoi(es.at(4)) // x y
							);
							break;
						}
					}
				}
				else
				if (es[0] == "setturn") {	
					switch (es.size()) {
						default: {
							cout << "Usage1: setturn num" << endl;
							break;
						}
						case 2: {
							env.turn_no = stoi(es.at(1));
							break;
						}
					}
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
				if (es.at(0) == "save") {
					switch (es.size()) {
						default: {
							cout << "Usage: save filename" << endl;
							break;
						}
						case 2: {
							ofstream f(es.at(1), std::ios::binary);
							io::write(f, env);
							break;
						}
					}
					
				}
				else
				if (es[0] == "addp") {
					if (es.size() < 2+1) {
						cout << "Usage: addp name color_name flag_id" << endl;
					}
					else {				
						env.add_player(Player(
							env.next_key_player(),
							string(es[1]),
							make_color_by_name(string(es[2])),
							stoi(es[3])
						));
					}
				}
				else
				if (es[0] == "set_owner") {
					env.set_owner(
						stoi(es[1]),
						stoi(es[2])
					);
				}
				else
				if (es.at(0) == "move") {					
					env.exec(action::Move(					
						stoi(es.at(1)), // id
						dir4vec(
							stoi(es.at(2)), // dx
							stoi(es.at(3))  // dy
						)
					));
				}
				else
				if (es.at(0) == "attack") {
					env.exec(action::Attack(					
						stoi(es.at(1)), // id
						dir4vec(
							stoi(es.at(2)), // dx
							stoi(es.at(3))  // dy
						)
					));
				}
				else
				if (es.at(0) == "enter") {
					switch (es.size()) {
						default: {
							cout << "Usage: enter [x y]" << endl;
							break;
						}
						case 1: {
							city_x = sel_x;
							city_y = sel_y;
							mod++;
							break;
						}
						case 3: {							
							city_x = stoi(es[1]);
							city_y = stoi(es[2]);
							mod++;
							break;
						}
					}
				}
				else
				if (es.at(0) == "exit") {
					switch (es.size()) {
						default: {
							cout << "Usage: exit" << endl;
							break;
						}
						case 1: {
							city_x = city_y = -1;
							mod++;
							break;
						}
					}					
				}
				else {
					cout << "ERROR: no such command" << endl;
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
	
	
	void render_city(sf::RenderWindow &win, const Env &env, const Console &con) {
		
		auto bg = res(ARCH, 1);
		
		auto w = bg.GetWidth();
		auto h = bg.GetHeight();
		
		for (uint8 j = 0; j < 5; ++j) {
			for (uint8 i = 0; i < 7; ++i) {
				render_sprite(win, i*w, 8+j*h, bg);
			}
		}
		
		// tree pattern
		uint16 TREES = 43;
		uint16 HILL = 44;
		uint16 TWO = 45;
		uint16 COAST = 46;
		uint16 B1 = TWO;
		uint16 B2 = HILL;
		uint16 B3 = TREES;
		
		vector<Coords> coords({
			Coords(7,15), Coords(54,22), Coords(90,13), Coords(144,13), Coords(172,15),
			Coords(7,42), Coords(37,45), Coords(66,52), Coords(96,52), Coords(129,54),
			Coords(9,77), Coords(14,100), Coords(68,87), Coords(124,58), Coords(121,107)
		});
		
		vector<uint16> trees{
			HILL, B1, B3, B1, B1,
			B1, TWO, B1, TWO, HILL,
			HILL, B2, TREES, COAST, B2
		};
		
		
		uint i = 0;
		for (auto &iicon: env.icons) {
			auto & icon = iicon.second;
			if (icon.x != con.city_x || icon.y != con.city_y) {
				continue;
			}
			
			auto type = icon.type->id;
			if (type < 256 || icon.x != con.city_x || icon.y != con.city_y) {
				continue;
			}
			
			auto & pos = coords[i];
			switch (type/ 256) {
				case 1:
					render_sprite(win, pos[0], pos[1], res(BUILDING, type % 255));
					break;
				default:
					render_sprite(win, pos[0], pos[1], res(BUILDING, -1));
					break;
			}
			
			
			cout << icon << endl;
			++i;
		}
		
		for (; i<15; ++i) {		
			auto & pos = coords[i];
			render_sprite(win, pos[0], pos[1], res(BUILDING, trees[i]));
		}
		
		
		
	}
	
	void render_tile(sf::RenderWindow &win, 
			Coord const& i, Coord const& j, 
			Terr const& tile) 
	{
		
		auto terrain = tile & 0x0F;
		auto forest = bool(tile & (1 << 4));
		
		
		render_sprite(win, i*16, j*16, res(TERR, terrain));
		if (forest) {
			//cout << uint16(tile) << endl;
			//cout << uint16(terrain) << endl;
			//cout << uint16(forest) << endl;
			//cout << endl;
			render_sprite(win, i*16, j*16, res(PHYS, 65));
		}
	}
	
	void render_map(sf::RenderWindow &win, const Env &env, const Console &con) 
	{
		auto w = env.w;
		auto h = env.h;

		for (uint j = 0; j < h; ++j) {
			for (uint i = 0; i < w; ++i) {
				render_tile(win, i, j, env.get_terr_id(i,j));
			}
		}

		for (auto &p: env.icons) {
			render_icon(win, env, p.second);
		}
		
		render_cursor(win, con.sel_x*16, con.sel_y*16);

	}

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
		else
		if (ev.Type == sf::Event::MouseButtonPressed)
		{
			if (ev.MouseButton.Button == sf::Mouse::Right)
			{
				sf::Vector2f mp = app.ConvertCoords(ev.MouseButton.X, ev.MouseButton.Y);
				con.sel_x = mp.x / 16;
				con.sel_y = mp.y / 16;
				++con.mod;
				
				std::cout << "sel x: " << con.sel_x << std::endl;
				std::cout << "sel y: " << con.sel_y << std::endl;				
			}
		}
		
	}

}


void render(sf::RenderWindow &app, const col::Env &env, const Text &t, const col::Console &con) {
	app.Clear();
	cout << con.city_x << endl;
	if (con.city_x != col::Coord(-1)) {
		render_city(app, env, con);
	}
	else {
		render_map(app, env, con);
	}
	render_text(app, t);
	render_playerind(app, 0, 0, env.curr_player->color);

	
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
	
	auto last_mod1 = env.mod - 1;
	auto last_mod2 = con.mod - 1;
	
	while (app.IsOpened())
	{
		
		if (env.mod != last_mod1 || con.mod != last_mod2) {
			render(app, env, t, con);
			last_mod1 = env.mod;
			last_mod2 = con.mod;
		}
		
		sf::Event ev;
		while (app.GetEvent(ev)) {
			handle_event(app, env, ev, con);
		}		
		
	}

	/*{
		ofstream fo(fname, std::ios::binary);
		io::write<Env>(fo, env);
	}*/
		
	return EXIT_SUCCESS;
}

