#ifndef COL_H
#define COL_H

#include "pre.h"

#include <typeinfo>
#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <utility>
#include <tuple>
#include <algorithm>
#include <boost/multi_array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/serialization/split_member.hpp>
#include "roll.h"
#include "aga2.hpp"
#include "format.hpp"

namespace col{


	using std::ifstream;
	using std::ofstream;
	using std::string;
	using std::cerr;
	using std::to_string;
	using std::function;
	using std::unordered_set;
	using std::unique_ptr;
	using std::shared_ptr;
	using std::make_shared;
	using std::unordered_map;
	using std::array;

	using std::cout;
	using std::endl;

	using std::pair;
	using std::map;
	using std::vector;
	using std::remove;

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

	using std::piecewise_construct;
	using std::forward_as_tuple;

	using std::ostream;
	using std::istream;
	using std::size_t;

	using std::uint8_t;

	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint = std::uint32_t;

	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;

	using format::format;

	template <typename T>
	using Vector2 = aga2::Mv1<T>;

	using v2i = Vector2<int>;
	using v2f = Vector2<float>;

	using Amount = int16;

	//template <typename T>
	//unique_ptr<T> make_unique()

}




#endif