#include "roll.h"

namespace roll {
	
	boost::random::mt19937 gen;

	int roll(int b) {
		boost::random::uniform_int_distribution<> dist(0, b-1);
		return dist(gen);
	}
}
