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

using std::unique_ptr;

using uint8 = boost::uint8_t;
using uint16 = boost::uint16_t;
using uint32 = boost::uint32_t;

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

//------------------------------------------------------------------------------



using Res = map<pair<string,uint>,sf::Image>;
Res g_res;

const string 
	TERR = "TERRAIN_SS",
	ICON = "ICON_SS";

const Res::mapped_type& res(const string &d, const uint &i) {
	
	auto key = Res::key_type(d,i);
	auto p = g_res.find(key);	
	if (p != g_res.end()) {
		return (*p).second;
	}
	else {
		auto &img = g_res[key];
		auto path = str(format("./COLONIZE/%||/%|03|.png") % d % i);
		if (!img.LoadFromFile(path)) {
			throw std::runtime_error("cant't load image file: " + path);
		}
		return img;		
	}
}




namespace iomm{
	
	
	
	char read_byte(ifstream &inn) {
		char c = inn.get();
		if (!inn.good()) {
			throw std::runtime_error(string("error while reading from stream"));
		}
		return c;
	}

	uint16 read_uint16(ifstream& f) {
		uint16 a = read_byte(f);
		uint16 b = read_byte(f);
		return (b << 8) | a;
	}

	uint8 read_uint8(ifstream& f) {
		return read_byte(f);	
	}

	string read_ascii(ifstream& f, uint32 n) {
		string s;
		s.reserve(n);
		uint32 i = 0;
		while (i < n) {
			char c = read_byte(f);
			assert(0 <= c && c <= 127);  // ascii range
			s.push_back(c);
			++i;
		}
		return s;
	}
	
	void write_byte(ofstream &f, char x) {
		f.put(x);		
	}

	void write_uint16(ofstream& f, uint16 x) {
		write_byte(f, x & 0xff);
		write_byte(f, x >> 8);		
	}

	void write_uint8(ofstream& f, uint8 x) {
		write_byte(f, x);	
	}

	void write_ascii(ofstream& f, string s) {
		uint32 i = 0;
		while (i < s.size()) {
			write_byte(f, s[i]);
			++i;
		}
	}
}
 
using namespace iomm;

struct Env{
	using Terrain = boost::multi_array<uint8, 2>;
	
	uint w, h;
	Terrain terr;
	
};

Env read_env(ifstream &f) {
	Env env;
	auto &terrain = env.terr;
	
	string fid = "COLMAPB";
	string ind = read_ascii(f, fid.size());
	assert(ind == fid);
	auto version = read_uint8(f);
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
	
	return env;
}


//[[[cog
//   print('aaa')
//]]]
aaa
//[[[end]]]
		
		
Icon
uint32 id;
uint16 type
uint16 x;
uint16 y;

struct Icon{
	uint id;
	uint type;
	uint x,y;
};

Icon read_icon(f) {
	Icon icon;
	icon.id = read_uint32(f);
	icon.type = read_uint16(f);
	icon.x = read_uint16(f);
	icon.y = read_uint16(f);
	return icon;
}

void read_icon(f, const Icon &icon) {	
	icon.id = read_uint32(f);
	icon.type = read_uint16(f);
	icon.x = read_uint16(f);
	icon.y = read_uint16(f);
}

void write_env(ofstream &f, const Env &env) {
	auto &terrain = env.terr;
	uint w = env.w;
	uint h = env.h;
	
	
	uint8 version = 1;
	string fid = "COLMAPB";
	
	write_ascii(f, fid);
	write_uint8(f, version);
	
	write_uint16(f, w);
	write_uint16(f, h);
		
	for(uint j = 0; j < h; ++j) {
		for(uint i = 0; i < w; ++i) {
			write_uint8(f, terrain[i][j]);
		}
	}
	
	write_uint16(f, 1); // num of units on map
	res(ICON, 100)

}





void render_map(sf::RenderWindow &win, const Env &env) 
{
	auto &terrain = env.terr;
	
	auto w = env.w;
	auto h = env.h;
	
	for (uint i = 0; i < w; ++i) {
		for (uint j = 0; j < h; ++j) {
			sf::Sprite s;
			s.SetPosition(i*16-0.5, j*16-0.5);
			s.SetImage(res(TERR, terrain[i][j]));
			win.Draw(s);
		}
	}

}




int main()
{
	string fname("./aaa.mp");
	
	const uint SCL = 3;
	sf::RenderWindow app(sf::VideoMode(320 * SCL, 200 * SCL, 32), "SFML Window");
	sf::View view(sf::FloatRect(0, 0, 320, 200));
	app.SetView(view);

	
	ifstream f(fname, std::ios::binary);
	auto env = read_env(f);
	f.close();

    
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
			if (ev.Type == sf::Event::Closed) {
				app.Close();
			}
		}
		
		app.Clear();
		
		render_map(app, env);
		
		app.Display();
	}

	{
		ofstream fo(fname, std::ios::binary);
		write_env(fo, env);
	}
		
	return EXIT_SUCCESS;
}

