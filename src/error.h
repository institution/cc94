#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>
#include <string>

namespace col {


	class Error: public std::runtime_error {
		public:
			explicit Error (const std::string& what_arg): std::runtime_error(what_arg) {}
			explicit Error (const char* what_arg): std::runtime_error(what_arg) {}
	};

	

}

#endif
