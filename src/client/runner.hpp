#pragma once

#include "agent.hpp"
#include "env.hpp"


namespace col{


	

	struct Runner {
		
		Env env;
		using Agents = vector<Agent*>;
		Agents agents;
		int verbose{0};
		
		static Agents::size_type const max_size;
		
		Runner() {
			agents.reserve(max_size);
		}
		
		void init(std::string const& file_map, std::string const& dir_tile, int verbose);


		template <class T, class ... Args>
		void add_agent(Args && ... args) {
			if (agents.size() < max_size) {
				agents.push_back(new T(args...));				
			}
			else {
				ext::fail("cannot add agent: agents.size() >= max_size");
			}
		}

		~Runner() {
			for (auto p: agents) {
				delete p;
			}
		}

		bool step();
		

	};

}

