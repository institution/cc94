#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <vector>
#include <map>
#include <boost/cstdint.hpp>
#include <boost/multi_array.hpp>
#include <boost/format.hpp>
#include "uint.h"
#include "iomm.h"
#include "text.h"

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

using namespace iomm;


//------------------------------------------------------------------------------



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





 
using namespace iomm;


/*[[[cog
import meta;
meta.rwstruct('Icon',
	'uint32 id',
	'uint16 type',
	'uint16 x',
	'uint16 y',
)
]]]*/
struct Icon {
	uint32 id;
	uint16 type;
	uint16 x;
	uint16 y;

	Icon() {}
	Icon(uint32 id_, uint16 type_, uint16 x_, uint16 y_): id(id_), type(type_), x(x_), y(y_) {}
}; //Icon

Icon read_icon(ifstream &f) {
	Icon icon;
	icon.id = read_uint32(f);
	icon.type = read_uint16(f);
	icon.x = read_uint16(f);
	icon.y = read_uint16(f);
	return icon;
}

void write_icon(ofstream &f, const Icon &icon) {
	write_uint32(f, icon.id);
	write_uint16(f, icon.type);
	write_uint16(f, icon.x);
	write_uint16(f, icon.y);
}

//[[[end]]]
		

struct Env{
	using Terrain = boost::multi_array<uint8, 2>;
	
	uint w, h;
	Terrain terr;
	vector<Icon> icons;
};

Env read_env(ifstream &f) {
	Env env;
	auto &terrain = env.terr;
	
	string fid = "COLMAPB";
	string ind = read_ascii(f, fid.size());
	assert(ind == fid);
	auto version = read_uint8(f);
	auto nsect = read_uint8(f);
	
	if (nsect < 1) return env;
	// terrain
	auto w = read_uint16(f);
	auto h = read_uint16(f);
	env.w = w;
	env.h = h;
	
	cout << format("%1% %2%\n") % w % h;
	terrain.resize(boost::extents[w][h]);

	for(uint j = 0; j<h; ++j) {
		for(uint i = 0; i<w; ++i) {
			terrain[i][j] = read_byte(f);
		}
	}
	
	if (nsect < 2) return env;
	// icons	
	uint nunits = read_uint16(f); // num of units on map
	cout << format("nunits = %1%\n") % nunits;
	for (uint i = 0; i < nunits; ++i) {
		env.icons.push_back(read_icon(f));
	}
	
	return env;
}





void write_env(ofstream &f, const Env &env) {
	auto &terrain = env.terr;
	uint w = env.w;
	uint h = env.h;
	
	
	uint8 version = 1;
	uint8 nsect = 2;
	string fid = "COLMAPB";
	
	write_ascii(f, fid);
	write_uint8(f, version);
	write_uint8(f, nsect);
	
	// terrain
	write_uint16(f, w);
	write_uint16(f, h);
		
	for(uint j = 0; j < h; ++j) {
		for(uint i = 0; i < w; ++i) {
			write_uint8(f, terrain[i][j]);
		}
	}
	
	// icons
	uint nunits = env.icons.size();
	write_uint16(f, nunits); // num of units on map
	for (auto &icon: env.icons) {
		write_icon(f, icon);
	}
		
	//res(ICON, 100)

}





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
	
	for (auto &icon: env.icons) {
		render_icon(win, icon);
	}

}


int main()
{
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

    
	
	
	app.Clear();

	render_map(app, env);
	render_text(app, t);

	app.Display();
	
	while (app.IsOpened())
	{
		sf::Event ev;
		while (app.GetEvent(ev)) {
			if (ev.Type == sf::Event::KeyPressed) {
				if (ev.Key.Code == sf::Key::Escape) {
					app.Close();
				}				
			}
			else
			if (ev.Type == sf::Event::KeyReleased) {
				if (ev.Key.Code == sf::Key::C) {
					env.icons.push_back(
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

	{
		ofstream fo(fname, std::ios::binary);
		write_env(fo, env);
	}
		
	return EXIT_SUCCESS;
}

