#ifndef ERROR_H
#define ERROR_H


namespace col{
	

	class Error: public runtime_error {
		public:
			explicit Error (const string& what_arg);
			explicit Error (const char* what_arg);
	};


}

#endif
