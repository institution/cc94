#include "human_ai.h"

#include <exception>
#include <mutex>

#include "renderer.h"


namespace human_ai{
		
	void run(Nation::Id pid, EnvGame * s_env, bool * running) {
		
		/*
		ExpertAi ai;
		env->connect(pid, ai);
		ai.mtx.lock();
		
		while (1) {
			ai.mtx.lock();
			
			if (*running) {
				std::cout << "gui client active" << std::endl;
				// TODO ... send some commands
				env->exec(Ready(pid));
			}
			else {
				break;
			}

			//std::this_thread::sleep_for(std::chrono::seconds(2));
		}
		*/
		
				// ee -- server

		
	
	
		
		EnvGame env;
		col::Console con("human_ai", env, nullptr, *running, pid);
		s_env->connect(pid, con);

		//preload_terrain();

		sf::RenderWindow app(
			sf::VideoMode(col::SCREEN_W * col::GLOBAL_SCALE, col::SCREEN_H * col::GLOBAL_SCALE, 32),
			"cc14"
		);

		sf::View view(sf::FloatRect(0, 0, col::SCREEN_W, col::SCREEN_H));
		app.setView(view);
		
		auto last_env = con.env.mod - 1;
		auto last_con = con.mod - 1;

		sf::Clock clock;
		auto last_t = clock.getElapsedTime().asSeconds();
		while (app.isOpen())
		{
			if ((con.env.mod != last_env) || (con.mod != last_con) || (last_t + 0.1 > clock.getElapsedTime().asSeconds())) {
				//cout << "RENDER:" << con.mod << ',' << env.mod << endl;
				con.time = clock.getElapsedTime().asSeconds();
				last_t = con.time;

				col::render(app, con.env, con);

				last_env = con.env.mod;
				last_con = con.mod;
			}

			con.handle_events(app);

		}
		
		std::cout << "gui -- clean exit\n";
		
		
	}

	
	
	
	
	
	
	
	
}
