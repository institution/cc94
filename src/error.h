#ifndef ERROR_H
#define ERROR_H


namespace col{
	

	class Error: public runtime_error {
		public:
			explicit Error (const string& what_arg);
			explicit Error (const char* what_arg);
	};

	
	using uint16 = ErrNo;	
	ErrNo const NOT_ENOUGH_SPACE = 2;
	ErrNo const CANNOT_MOVE = 3;
	
	
	//ErrNo const  = ;
}

#endif
