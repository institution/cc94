#ifndef CSV_H
#define CSV_H

//#define BOOST_SPIRIT_DEBUG
#include <boost/spirit/include/qi.hpp>

#include "col.h"


namespace col {
	
	vector<vector<string>> read_conf(const string &fname);
			
}

#endif //CSV_H