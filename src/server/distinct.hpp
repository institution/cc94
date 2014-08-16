#ifndef DISTINCT2_H
#define DISTINCT2_H

#include <limits>
#include <memory>
#include <functional>

namespace distinct {

	template<typename U, typename T>
	struct Enum{
		T val{0};

		using domain_type = U;
		using value_type = T;

		using Id = Enum<U,T>;

		Enum() {}

		constexpr
		Enum(T x): val(x) {
			//static_assert(std::numeric_limits<T>::min() <= x);
			//static_assert(x <= std::numeric_limits<T>::max());
		}

		constexpr
		operator T() const {
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
			size_t operator()(Enum<U,T> const& x) const
			{
				return std::hash<decltype(x.val)>()(x.val);
			}
		};

	};

	template<class U,class T>
	bool operator==(Enum<U,T> const& x, Enum<U,T> const& y) {
		return x.get_value() == y.get_value();
	}

	template<class U,class T>
	bool operator!=(Enum<U,T> const& x, Enum<U,T> const& y) {
		return x.get_value() != y.get_value();
	}

	template<class U,class T>
	bool operator<=(Enum<U,T> const& x, Enum<U,T> const& y) {
		return x.get_value() <= y.get_value();
	}

	template<class U,class T>
	bool operator<(Enum<U,T> const& x, Enum<U,T> const& y) {
		return x.get_value() < y.get_value();
	}


	template<typename U, typename T>
	struct Flag: Enum<U,T> {
		using Enum<U,T>::Enum;

		operator bool() const {
			return bool(Enum<U,T>::get_value());
		}
	};

	template<class U,class T>
	Flag<U,T> operator~(Flag<U,T> const& x) {
		return Flag<U,T>(~x.get_value());
	}

	template<class U,class T>
	Flag<U,T> operator|(Flag<U,T> const& x, Flag<U,T> const& y) {
		return Flag<U,T>(x.get_value() | y.get_value());
	}

	template<class U,class T>
	Flag<U,T> operator&(Flag<U,T> const& x, Flag<U,T> const& y) {
		return Flag<U,T>(x.get_value() & y.get_value());
	}

}



namespace std {
	template<class U, class T>
	struct hash< distinct::Enum<U,T> >
	{
		size_t operator()(distinct::Enum<U,T> const& x) const
		{
			return std::hash<decltype(x.val)>()(x.val);
		}
	};
}


#endif
