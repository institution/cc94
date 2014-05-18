#ifndef ENUM2_H
#define ENUM2_H

#include <limits>
#include <memory>

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

	T const& get_value() const {
		return val;
	}

	template<class A>
	void serialize(A & ar, unsigned int version) {
		ar & val;
	}

	struct hash
	{
		size_t operator()(Enum<T> const& x) const
		{
			return x.val;
			//return std::hash<int>()(x.val);
		}
	};

};

template<typename T = int> class Enum;


namespace std {
	template <typename T>
	struct hash< Enum<T> >
	{
		size_t operator()(Enum<T> const& x) const
		{
			return x.val;
			//return hash<int>()(x.val);
		}
	};
}

#endif
