#ifndef GAME_H_0873498576
#define GAME_H_0873498576


#include "player.h"
#include "env.h"

namespace col{



	struct Players{
		vector<Player*> players;

		template <class T, class ...Args>
		void add(Args&... args) {
			players.push_back(new T(args...));
		}

		Player * first() {
			return players.at(0);
		}

		~Players() {
			for (auto p: players) {
				delete p;
			}
		}

	};



	struct Game {
		
		Players players;
		Env env;

		using Who = std::vector< std::pair<Nation::Id, Player*> > ;
		Who who;

		int verbose{0};
		
		
		void init(std::string const& fname, int verbose);
		

		void connect(Player * p, Nation::Id nation_id);

		template <class T, class ...Args>
		void add_player(Nation::Id nid, Args&... args) {
			players.add<T>(args...);
			connect(
				players.players.at(players.players.size() - 1),
				nid
			);
		}

		bool step();
		

	};

}


#endif
