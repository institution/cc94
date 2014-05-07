#ifndef ENUM2_H
#define ENUM2_H

#include <limits>

template<typename T>
struct Enum{
	T val;

	constexpr
	Enum(int x): val(x) {
		//static_assert(std::numeric_limits<T>::min() <= x);
		//static_assert(x <= std::numeric_limits<T>::max());
	}

	constexpr
	operator int() const {
		return val;
	}



	template<class A>
	void serialize(A & ar, unsigned int version) {
		ar & val;
	}

};

template<typename T = int> class Enum;


namespace std {
	template <typename T>
	struct hash< Enum<T> >
	{
		size_t operator()(Enum<T> const& x) const
		{
			return hash<int>()(x.val);
		}
	};
}

#endif
