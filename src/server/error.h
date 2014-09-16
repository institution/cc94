#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>
#include <string>

namespace col {

	/*
	class RuntimeError: public std::runtime_error {
		public:
			explicit RuntimeError (const std::string & what_arg, char const* file = "", int line = 0):
				std::runtime_error(what_arg) {}
			explicit RuntimeError (const char* what_arg, char const* file = "", int line = 0):
				std::runtime_error(what_arg) {}
	};*/


	class Critical: public std::runtime_error {
		public:
			explicit Critical (const std::string& what_arg): std::runtime_error(what_arg) {}
			explicit Critical (const char* what_arg): std::runtime_error(what_arg) {}
	};

	class Error: public std::runtime_error {
		public:
			explicit Error (const std::string & what_arg):
				std::runtime_error(what_arg) {}
			explicit Error (const char* what_arg):
				std::runtime_error(what_arg) {}
	};



}

#endif
