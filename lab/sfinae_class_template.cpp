#include "pch.h"
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include <boost/property_tree/ptree.hpp>

#include "mpl.h"

//////////////////////////////////////////////////////////////////////////

static std::string _output;
static boost::property_tree::ptree _ptree;

//////////////////////////////////////////////////////////////////////////

template <class S, class T, class = void>
struct gate
{
	static void process_val( const T& value )
	{
		_output = "general";
		_ptree.add( "value", value );
	}
};

template <class S, class T>
struct gate< S, T, typename std::enable_if_t<std::is_floating_point_v<T>> >
{
	static void process_val( const T& value )
	{
		_output = "double";
		_ptree.add( "value", value );
	}
};

template <class S, class T>
struct gate< S, T, typename std::enable_if_t<std::is_enum_v<T>> >
{
	static void process_val( const T& value )
	{
		_output = "enum";
		_ptree.add( "value", (long&)value );
	}
};

template <class S, class T>
struct gate< S, T, typename mpl::is_range<T> >
{
	static void process_val( const T& container )
	{
		_output = "range";
		for ( const auto& val : container )
			gate<void,decltype(val)>().process_val( val );
	}
};

template <class T>
void process( const T& val )
{
	gate<void,T>::process_val( val );
}

BOOST_AUTO_TEST_CASE( lab )
{
	{
		int i = 273;
		process( i );
		BOOST_CHECK_EQUAL( _output, "general" );
		BOOST_CHECK_EQUAL( _ptree.get<int>( "value" ), 273 );
	}
	{
		double d = 3.14;
		process( d );
		BOOST_CHECK_EQUAL( _output, "double" );
	}
	{
		enum enum_t { enum1, enum2 };
		enum_t e = enum2;
		process( e );
		BOOST_CHECK_EQUAL( _output, "enum" );
		//BOOST_CHECK( _ptree.get<long>("value") == enum2 );
	}
	{
		std::vector<int> v = { 1, 2, 3 };
		process( v );
		//BOOST_CHECK_EQUAL( _output, "range" );
	}
}