#include "pch.h"
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include <boost/property_tree/ptree.hpp>

#include "mpl.h"

//////////////////////////////////////////////////////////////////////////

/* Target types:
- fundamental
- string/wstring
- CNGPString
- enum
- static array
- vector
- set
- map
- ngp_shared_array
- shared_ptr
- custom struct
*/

// custom types
enum enum_t { enum1, enum2 };

//////////////////////////////////////////////////////////////////////////

static std::string _output;
static boost::property_tree::ptree _ptree;

//////////////////////////////////////////////////////////////////////////

// basic declarations
namespace prc
{
	template <class T, class = void>
	struct copy_gate
	{
		static void process( T& lhs, const T& rhs )
		{
			_output = "copy: generic";
			lhs = rhs;
		}
	};

	template <class T>
	void op_copy( T& lhs, const T& rhs )
	{
		copy_gate<T>::process( lhs, rhs );
	}

	template <class T, class = void>
	struct equal_gate
	{
		static bool process( const T& lhs, const T& rhs )
		{
			_output = "equal: generic";
			return lhs == rhs;
		}
	};

	template <class T>
	bool op_equal( const T& lhs, const T& rhs )
	{
		return equal_gate<T>::process( lhs, rhs );
	}
}

//////////////////////////////////////////////////////////////////////////

// 'copy' specific implementations
namespace prc
{
	template <class T>
	struct copy_gate< T, typename mpl::is_range<T> >
	{
		static void process( T& lhs, const T& rhs )
		{
			_output = "range";
			lhs.assign( rhs.begin(), rhs.end() );
		}
	};
}

// 'equal' specific implementations
namespace prc
{
	template <class T>
	struct equal_gate< T, typename mpl::is_range<T> >
	{
		static bool process( const T& lhs, const T& rhs )
		{
			_output = __FUNCTION__;
			if ( lhs.size() != rhs.size() )
				return false;
			return std::equal( lhs.cbegin(), lhs.cend(), rhs.cbegin() );
		}
	};
}

BOOST_AUTO_TEST_CASE( test_copy_equal )
{
	{
		const int i = 273;			int j;
		prc::op_copy( j, i );
		BOOST_CHECK( prc::op_equal( i, j ) );
	}
	{
		const double d = 3.14;		double dd;
		prc::op_copy( dd, d );
		BOOST_CHECK( prc::op_equal( d, dd ) );
	}
	{
		const enum_t e = enum2;		enum_t ee;
		prc::op_copy( ee, e );
		BOOST_CHECK( prc::op_equal( e, ee ) );
	}
	{
		const std::wstring src = L"my string";			std::wstring dst;
		prc::op_copy( dst, src );
		BOOST_CHECK( prc::op_equal( dst, src ) );
	}
	{
		std::vector<int> v = { 1, 2, 3 }, vv;
		prc::op_copy( vv, v );
		BOOST_CHECK( prc::op_equal( v, vv ) );
	}
	{
		const std::string src = "my string";			std::string dst;
		prc::op_copy( dst, src );
		BOOST_CHECK( prc::op_equal( dst, src ) );
	}
}