#ifndef ROLL_H
#define ROLL_H

#include <ctime>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace roll{

	extern boost::random::mt19937 gen;

	inline int roll(int a, int b) {
		boost::random::uniform_int_distribution<> dist(a, b-1);
		return dist(gen);
	}
	
	inline void seed() {
		gen.seed(std::time(0));
	}
}

#endif
