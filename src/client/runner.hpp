#pragma once

#include "agent.hpp"
#include "env.hpp"
#include <thread>



namespace col{	

	struct Runner
	{
		Env env;
		std::vector<Agent*> agents;
		std::vector<std::thread> threads;
		int verbose{0};		
		size_t const max_size{32};
		bool f_stop{false};
		
		Runner() {
			agents.reserve(max_size);
		}	

			
		template <class T, class ... Args>
		void add_agent(Args&& ... args)
		{
			if (agents.size() >= max_size) {
				ext::fail("cannot add agent: agents.size() >= max_size");
			}		
				
			agents.push_back(new T(std::forward<Args>(args)...));
			Agent * a = agents.back();
					
			threads.emplace_back(&Agent::run, std::ref(*a));
		};

		~Runner() {
			for (Agent * a: agents)
			{
				delete a;
			}
			agents.clear();
			threads.clear();
		}

		void run(std::string const& file_map, std::string const& dir_tile, int verbose);
		
	};

}

