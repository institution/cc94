#ifndef FORMAT123_H
#define FORMAT123_H

#include <string>
#include <boost/format.hpp>


inline
std::string format(boost::format pattern) {
	return pattern.str();
}

template <class U, class ...T> inline
std::string format(boost::format pattern, U&& u, T&&... rest) {
	return format(pattern % u, rest...);
}

template <class ...T> inline
std::string format(std::string pattern, T&&... rest) {
	return format(boost::format(pattern), rest...);
}






#endif
