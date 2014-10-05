#ifndef GAME_0873498576
#define GAME_0873498576


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
		Env & env;

		bool running;
		map<Nation::Id, Player*> who;

		Game(Env & env): env(env), running{true} {

		}

		void connect(Player * p, Nation::Id nation_id) {
			who[nation_id] = p;
			p->start();
		}

		template <class T, class ...Args>
		void add_player(Nation::Id nid, Args&... args) {
			players.add<T>(args...);
			connect(
				players.players.at(players.players.size() - 1),
				nid
			);
		}

		void stop() {
			running = false;
		}

		void play() {

			while (running and !env.in_progress()) {
				Player & player = *players.first();
				player.play(env, 0, 0);
			}

			while (running and env.in_progress()) {
				auto nation_id = env.get_id(env.get_current_nation());

				auto it = who.find(nation_id);
				Player *player;
				if (it != who.end()) {
					player = (*it).second;
				}
				else {
					player = players.first();
				}

				player->play(env, nation_id, env.get<Nation>(nation_id).auth);
			}

			for (Player* p: players.players) {
				p->stop();
			}


		}


	};

}


#endif
