#include "pch.h"
#include <boost/test/unit_test.hpp>

#include <type_traits>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

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
enum enum_t { enum1, enum2, enum3, enum4, enum5 };

static const std::vector<int> ref_vec_of_ints = { 1, 2, 3 };
static const std::vector<enum_t> ref_vec_of_enums = { enum1, enum2, enum3 };
static const std::vector<std::vector<int>> ref_vec_of_vec_of_ints = { {1,2,3}, {4,5} };

//////////////////////////////////////////////////////////////////////////

static std::string _output;
static boost::property_tree::wptree _ptree;

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

//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////

namespace prc
{
	template <class S, class T, class = void>
	struct io_gate
	{
		static void save( S& stream, const wchar_t* tag, const T& value )
		{
			stream << value;
		}
		static void load( const S& stream, const wchar_t* tag, T& value )
		{
			stream >> value;
		}
	};

	template <class S, class T>
	void op_save( S& stream, const wchar_t* tag, const T& value )
	{
		io_gate<S,T>::save( stream, tag, value );
	}

	template <class S, class T>
	void op_load( const S& stream, const wchar_t* tag, T& value )
	{
		io_gate<S,T>::load( stream, tag, value );
	}
}

// ptree
namespace prc
{
	template <class T, class = void>
	struct io_ptree_single
	{
		static void save( boost::property_tree::wptree& node, const T& value )
		{
			node.put_value( value );
		}
		static void load( const boost::property_tree::wptree& node, T& value )
		{
			if ( boost::optional<T> node_val = node.get_value_optional<T>() )
				value = *node_val;
		}
	};

	template <class T>
	struct io_ptree_single< T, typename std::enable_if_t<std::is_enum_v<T>> >
	{
		static void save( boost::property_tree::wptree& node, const T& value )
		{
			node.put_value( (long&)value );
		}
		static void load( const boost::property_tree::wptree& node, T& value )
		{
			if ( boost::optional<long> node_val = node.get_value_optional<long>() )
				value = (T)*node_val;
		}
	};

	template <class T>
	struct io_gate< boost::property_tree::wptree, T >
	{
		static void save( boost::property_tree::wptree& stream, const wchar_t* tag, const T& value )
		{
			boost::property_tree::wptree node;
			io_ptree_single<T>::save( node, value );
			stream.put_child( tag, node );
		}
		static void load( const boost::property_tree::wptree& stream, const wchar_t* tag, T& value )
		{
			boost::optional<const boost::property_tree::wptree&> node = stream.get_child_optional( tag );
			if ( node )
			{
				io_ptree_single<T>::load( *node, value );
			}
		}
	};

	template <class T>
	struct io_gate< boost::property_tree::wptree, std::vector<T> >
	{
		using CONTAINER = std::vector<T>;
		static void save( boost::property_tree::wptree& stream, const wchar_t* tag, const CONTAINER& container )
		{
			boost::property_tree::wptree array_node;
			for ( const auto& value : container )
			{
				boost::property_tree::wptree prop_node;
				//io_gate<boost::property_tree::wptree,T>::save( prop_node, , value );
				io_ptree_single<T>::save( prop_node, value );
				array_node.add_child( L"item", prop_node );
			}
			stream.put_child( tag, array_node );
		}
		static void load( const boost::property_tree::wptree& stream, const wchar_t* tag, CONTAINER& container )
		{
			boost::optional<const boost::property_tree::wptree&> array_node = stream.get_child_optional( tag );
			if ( array_node )
			{
				container.clear();

				for ( const boost::property_tree::wptree::value_type& item_node : *array_node )
				{
					_ASSERTE( item_node.first == L"item" );
					T item;
					io_ptree_single<T>::load( item_node.second, item );
					container.push_back( item );
				}
			}
		}
	};
}

#define TEST_IO_FILE( T, ref_value, tag, filename ) \
	{ \
		const T rhs = (ref_value); \
		T lhs; \
		boost::property_tree::wptree pt; \
		prc::op_save( pt, tag, rhs ); \
		prc::op_load( pt, tag, lhs ); \
		BOOST_CHECK( prc::op_equal( lhs, rhs ) ); \
		if ( filename ) { boost::property_tree::xml_writer_settings<std::wstring> xmlps; xmlps.indent_count = 4; boost::property_tree::write_xml( filename, pt, std::locale(), xmlps ); } \
	}

#define TEST_IO( T, ref_value, tag ) TEST_IO_FILE( T, ref_value, tag, nullptr )

BOOST_AUTO_TEST_CASE( test_ptree )
{
	TEST_IO( int, 273, L"my_int" );
	TEST_IO( double, 3.14, L"my_double" );
	TEST_IO( enum_t, enum2, L"my_enum" );
	TEST_IO( std::wstring, L"sample string", L"my_string" );
	TEST_IO_FILE( std::vector<int>, ref_vec_of_ints, L"ivec", "vec_of_ints.xml" );
	TEST_IO( std::vector<enum_t>, ref_vec_of_enums, L"evec" );
	//TEST_IO( std::vector<std::vector<int>>, ref_vec_of_vec_of_ints, L"ivecvec" );
// 	{
// 		std::vector<enum_t> v = { enum1, enum2, enum3 };
// 		std::vector<int> lhs;
// 		boost::property_tree::wptree pt;
// 		prc::op_save( pt, L"arr", rhs );
// 		prc::op_load( pt, L"arr", lhs );
// 		BOOST_CHECK( prc::op_equal( lhs, rhs ) );
// 	}
// 	{
// 		const std::string src = "my string";			std::string dst;
// 		prc::op_copy( dst, src );
// 		BOOST_CHECK( prc::op_equal( dst, src ) );
// 	}
}

//////////////////////////////////////////////////////////////////////////
