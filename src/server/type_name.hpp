#pragma once

namespace col{

	template<class T>
	struct type_name;

	namespace inter{

		template<class T>
		struct type_name;

	}
}

#define ENABLE_TYPENAME(A)                                                      \
template<>                                                                      \
struct type_name<A> {                                                           \
	static std::string get() {                                                  \
		return #A;                                                              \
	}                                                                           \
};                                                                              \


