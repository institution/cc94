#ifndef RUNNER_H_0873498576
#define RUNNER_H_0873498576


#include "agent.h"
#include "env.h"

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
		
		void init(std::string const& fname, int verbose);


		template <class T, class ... Args>
		void add_agent(Args & ... args) {
			if (agents.size() < max_size) {
				agents.push_back(new T(args...));
			}
			else {
				error("cannot add agent: agents.size() >= max_size");
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


#endif
