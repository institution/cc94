#pragma once

//#define BOOST_SPIRIT_DEBUG
#include <string>
#include <vector>
#include <boost/spirit/include/qi.hpp>




namespace col {

	using std::vector;
	using std::string;

	vector<vector<string>> read_conf(const string &fname);

}
