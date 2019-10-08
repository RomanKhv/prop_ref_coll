#pragma once

#include <type_traits>

namespace mpl
{
//////////////////////////////////////////////////////////////////////////

// namespace detail
// {
// template <class...> struct void_type { using type = void; };
// }
// 
// 	template <class... T>
// 	using void_t = typename detail::void_type<T...>::type;

//////////////////////////////////////////////////////////////////////////

	template <class T>
	using is_range = /*mpl*/std::void_t
		< decltype(std::declval<T>().begin())
		, decltype(std::declval<T>().end())
		, decltype(std::declval<T>().size()) >;

//////////////////////////////////////////////////////////////////////////
}