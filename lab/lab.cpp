#include "pch.h"
#include <boost/test/unit_test.hpp>
boost::unit_test::test_suite* init_unit_test_suite( int argc, char* argv[] ) { return nullptr; }

#include <type_traits>
#include <boost/property_tree/ptree.hpp>

//////////////////////////////////////////////////////////////////////////

std::string _output;
boost::property_tree::ptree _ptree;

//////////////////////////////////////////////////////////////////////////

template <typename T>
void process( const T& value )
{
	_output = "general";
	_ptree.put( "value", value );
}

template <typename T>
typename std::enable_if_t<std::is_enum<T>::value, T> process( const T& value )
{
	_output = "enum";
	_ptree.put( "value", (long&)value );
}


BOOST_AUTO_TEST_CASE( lab )
{
	{
		int i = 273;
		process( i );
		BOOST_CHECK_EQUAL( _output, "general" );
		BOOST_CHECK_EQUAL( _ptree.get<int>("value"), 273 );
	}
	{
		enum enum_t { enum1, enum2 };
		enum_t e = enum2;
		process( e );
		BOOST_CHECK( _ptree.get<long>("value") == enum2 );
	}
}