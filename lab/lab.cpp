#include "pch.h"
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include <boost/property_tree/ptree.hpp>

//////////////////////////////////////////////////////////////////////////

std::string _output;
boost::property_tree::ptree _ptree;

//////////////////////////////////////////////////////////////////////////

template <class S, typename T, class = void>
struct gate
{
	void process_val( const T& value )
	{
		_output = "general";
		_ptree.put( "value", value );
	}
};

template <class S, double, std::true_type>
struct gate
{
	void process_val( const double& value )
	{
		_output = "double";
		_ptree.put( "value", value );
	}
};

// template <class S, class T, std::enable_if_t<std::is_enum<T>::value>>
// struct gate
// {
// 	void process_val( const T& value )
// 	{
// 		_output = "enum";
// 		_ptree.put( "value", (long&)value );
// 	}
// };


template <class T>
void process( const T& val )
{
	gate<void,T>().process_val( val );
}

BOOST_AUTO_TEST_CASE( lab )
{
	{
		int i = 273;
		process( i );
		BOOST_CHECK_EQUAL( _output, "general" );
		BOOST_CHECK_EQUAL( _ptree.get<int>("value"), 273 );
	}
// 	{
// 		enum enum_t { enum1, enum2 };
// 		enum_t e = enum2;
// 		process( e );
// 		BOOST_CHECK( _ptree.get<long>("value") == enum2 );
// 	}
}