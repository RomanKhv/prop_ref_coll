#pragma once

#include <type_traits>

namespace mpl
{
//////////////////////////////////////////////////////////////////////////

	template <class T> struct is_shared_ptr                       : std::false_type {};
	template <class T> struct is_shared_ptr<std::shared_ptr<T>>   : std::true_type {};
	template <class T> struct is_shared_ptr<boost::shared_ptr<T>> : std::true_type {};

	template <class T> struct is_smart_ptr                       : std::false_type {};
	template <class T> struct is_smart_ptr<std::unique_ptr<T>>   : std::true_type {};
	template <class T> struct is_smart_ptr<std::shared_ptr<T>>   : std::true_type {};
	template <class T> struct is_smart_ptr<boost::scoped_ptr<T>> : std::true_type {};
	template <class T> struct is_smart_ptr<boost::shared_ptr<T>> : std::true_type {};

//////////////////////////////////////////////////////////////////////////

	template <class T>
	using is_range = /*mpl*/std::void_t
		< decltype(std::declval<T>().begin())
		, decltype(std::declval<T>().end())
		, decltype(std::declval<T>().size()) >;

//////////////////////////////////////////////////////////////////////////
}