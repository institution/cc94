#ifndef ERROR_H_8343675
#define ERROR_H_8343675

#include <stdexcept>
#include <string>

#include <format.hpp>



struct RuntimeError: std::runtime_error {
	explicit RuntimeError ():
		std::runtime_error("") {}
	
	explicit RuntimeError (const std::string & what_arg):
		std::runtime_error(what_arg) {}

	explicit RuntimeError (const char* what_arg):
		std::runtime_error(what_arg) {}

	template <class ... Args>
	explicit RuntimeError (const char* what_arg, Args... args):
		std::runtime_error(format(what_arg, args...)) {}
};

struct Error: RuntimeError {
	using RuntimeError::RuntimeError;
};


	


#endif
