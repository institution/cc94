#ifndef ROLL_H
#define ROLL_H

#include <ctime>
#include <vector>
#include <initializer_list>
#include <algorithm>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace roll{

	extern boost::random::mt19937 gen;

	inline int roll2(int a, int b) {
		boost::random::uniform_int_distribution<> dist(a, b-1);
		return dist(gen);
	}
	
	
	int roll1(int b);
	
	inline void seed() {
		gen.seed(std::time(0));
	}
	
	
	struct replay{
		int index;
		std::vector<int> nums;

		replay(std::initializer_list<int> const& xs): index(0) {
			nums.resize(xs.size());
			std::copy(xs.begin(), xs.end(), nums.begin());
		}

		int operator()(int range) {
			auto n = nums.at(index);
			assert(n < range);
			index += 1;
			return n;
		}
	};
	
	
}

#endif
