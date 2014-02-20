#ifndef COL_H
#define COL_H

#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <boost/multi_array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/optional.hpp>
#include "iomm.h"
#include "roll.h"
#include "aga2.hpp"

namespace col{
	
	using boost::format;
	using std::ifstream;
	using std::ofstream;
	using std::string;
	using std::to_string;
	using std::function;

	using std::cout;
	using std::endl;

	using std::pair;
	using std::map;
	using std::vector;
	
	using boost::algorithm::trim;	
	using boost::algorithm::trim_copy;
	using boost::none;
	using boost::optional;
	
	using std::stoi;
	using std::runtime_error;
	using std::remove_reference;
	
	using boost::split;
	using boost::is_any_of;
	using std::stoi;
	
	using roll::roll;
	
	template <typename T>
	using Vector2 = aga2::Mv1<T>;
	
	using Vector2i = Vector2<int16>;
	using Vector2f = Vector2<float>;
}

#endif