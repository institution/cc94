#ifndef COL_H
#define COL_H

#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <vector>
#include <string>
#include <map>
#include <boost/multi_array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "iomm.h"

namespace col{
	
	using boost::format;
	using std::ifstream;
	using std::ofstream;
	using std::string;
	using std::to_string;

	using std::cout;
	using std::endl;

	using std::pair;
	using std::map;
	using std::vector;
	
	using boost::algorithm::trim;	
	using boost::algorithm::trim_copy;
	using std::stoi;
	using std::runtime_error;
	
}

#endif