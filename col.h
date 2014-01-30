#ifndef COL_H
#define COL_H

#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <vector>
#include <map>
#include <boost/cstdint.hpp>
#include <boost/multi_array.hpp>
#include <boost/format.hpp>

#include "uint.h"


namespace col{
	using namespace iomm;

	using boost::format;
	using std::ifstream;
	using std::ofstream;
	using std::string;
	using std::add_pointer;
	using std::to_string;

	using std::cout;
	using std::endl;

	using std::pair;
	using std::map;
	using std::vector;

	
}

#endif