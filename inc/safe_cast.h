#ifndef SAFE_CAST238573_H
#define SAFE_CAST238573_H

#include <type_traits>


template<class From, class To>
using is_safe_cast = std::is_same<
	typename std::common_type<From, To>::type,
	To
>;

template<class From, class To>
using enable_if_safe_cast = std::enable_if<
	is_safe_cast<From, To>::value
>;


#endif
