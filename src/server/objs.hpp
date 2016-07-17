#pragma once

#include "col.hpp"
#include "base.hpp"



namespace col {


	inline int stoi0(string const& s, int none = 0) {
		if (trim_copy(s).empty()) {
			return none;
		}
		return stoi(s);
	}




}
