#pragma once
#include <array>
#include "../ext/format.hpp"

namespace col {
	template <class T, T End> inline
	T _get_by_name(std::array<char const*, End> const& names, char const* name) {
		for (T x = T(0); x < End; ++x) {
			if (strcmp(names.at(x), name) == 0) {
				return x;
			}
		}
		print(std::cerr, "INFO: get_by_name: invalid name: %||\n", name);
		return T(0);
	}
	
	template <class T, T End> inline
	T _get_by_name(std::array<char const*, End> const& names, std::string const& name) {
		return _get_by_name<T, End>(names, name.c_str());
	}
	
}
