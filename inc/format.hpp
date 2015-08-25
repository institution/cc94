#ifndef FORMAT_HPP_97856623
#define FORMAT_HPP_97856623

#include <string>
#include <iostream>
#include <boost/format.hpp>
#include <stdexcept>



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

template <class ... Args>
void print(Args ... args) {
	std::cout << format(args...);
}

template <class ... Args>
void print_out(Args ... args) {
	std::cout << format(args...);
}

template <class ... Args>
void print_err(Args ... args) {
	std::cerr << format(args...);
}




template <class ...Args>
void error(Args ... args) {
	print_err("ERROR: ");
	print_err(args...);
	print_err("\n");		
	throw std::runtime_error(format(args...));
}

inline
void error() {
	print_err("ERROR: \n");
	throw std::runtime_error("");
}


#endif
