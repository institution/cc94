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
#include "uint.h"
#include "iomm.h"
#include "objs.h"

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

	struct Env{


		uint w, h;
		Terrain terr;
		vector<Icon> icons;

		// uint cur_x, cur_y;  

		uint turn;

		vector<Player> players;   // ls of player id



	};


	struct Game: Env{

		Game(): Env() {
			players.push_back(Player(254, string("server")));
		}


	};






	/*
	void exec(Env &env, Action &a, long &auth_key)

	void new_icon(Env &env, const Icon &icon) {

		env.icons.push_back(icon);
		env.icons[env.icons.size()-1].id = env.icon

		uint32 id;
		uint16 type;
		uint16 x;
		uint16 y;

	}


	void turn(Env &env) {
		for (auto &p: env.players) {

			auto a = player_if.retrive();
			auto r = exec_action(a);
			player_if.send(r);

		}
		env.turn += 1;

	}

	*/





	Env read_env(ifstream &f) {
		Env env;
		auto &terrain = env.terr;

		string fid = "COLMAPB";
		string ind = read_chars(f, 7);
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

		if (nsect < 3) return env;
		// players
		uint nplayers = read_uint8(f);
		cout << format("nplayers = %1%\n") % nunits;
		for (uint i = 0; i < nplayers; ++i) {
			env.players.push_back(read_player(f));
		}

		return env;
	}





	void write_env(ofstream &f, const Env &env) {
		auto &terrain = env.terr;
		uint w = env.w;
		uint h = env.h;


		uint8 version = 1;
		uint8 nsect = 3;
		string fid = "COLMAPB";

		write_chars(f, fid);
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

		// players
		uint np = env.players.size();
		write_uint8(f, np);
		for (auto &p: env.players) {
			write_player(f, p);
		}

		//res(ICON, 100)

	}









	struct Net{

		sf::SocketUDP sock;

		Net() {
			if (!sock.Bind(4567)) {
				throw std::runtime_error("cannot bind to port 4567");
			}
		}

		void receive() {
			char buffer[512];
			std::size_t received_size;
			sf::IPAddress sender;
			unsigned short sender_port;

			auto stat = sock.Receive(
				buffer, sizeof(buffer), received_size, sender, sender_port
			);

			if (stat != sf::Socket::Done) {
				throw std::runtime_error("error on recive");
			}

			cout << format("received %|| bytes from %||\n") % received_size % sender;
			cout << buffer << endl;
		}

		~Net() {
			sock.Close();
		}

	};

}


int main()
{
	using namespace col;
	
	string fname("./aaa.mp");
	
	ifstream f(fname, std::ios::binary);
	auto env = read_env(f);
	f.close();

	Net net;
	
	while (1)
	{
		net.receive();		
	}

	{
		ofstream fo(fname, std::ios::binary);
		write_env(fo, env);
	}
		
	return 0;
}

