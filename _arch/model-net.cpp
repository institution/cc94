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
#include "col.hpp"
#include "iomm.h"
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
	
			

	
	// load
	// -> stored game info
	//




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
			
			/*
			istringstream inn(buffer);
			PacketHeader ph = read_packedheader(inn);
			ActionHeader ah = read_actionheader(inn);
			
			Action *act;
			switch (ah.action_id) {
				case LoadGame::sid:
					act = new LoadGame(inn);					
					break;
			}
			*/
			
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
	auto env = io::read<Env>(f);
	f.close();

	Net net;
	
	while (1)
	{
		net.receive();		
	}

	{
		ofstream fo(fname, std::ios::binary);
		io::write<Env>(fo, env);
	}
		
	return 0; 
}

