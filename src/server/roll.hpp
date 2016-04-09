#pragma once

#include <ctime>
#include <vector>
#include <initializer_list>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>



namespace roll{

	extern boost::random::mt19937 gen;

	inline int roll2(int a, int b) {
		boost::random::uniform_int_distribution<> dist(a, b-1);
		return dist(gen);
	}


	int roll1(int b);

	inline void seed() {


		boost::random_device seed_gen;
		gen.seed(seed_gen());

		/*std::ifstream f("/dev/random");
		int s;
		f.read((char*)(&s), sizeof(s));
		std::cout << "rand seed = " << s << std::endl;
		gen.seed(s);
		*/
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
